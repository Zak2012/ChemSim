#include "Widget.hpp"

// #define GLFW_EXPOSE_NATIVE_WIN32
// #include <GLFW/glfw3.h>
// #include <GLFW/glfw3native.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>

#define HOLD_THRESHOLD 500

// #include <windows.h>

// // static WNDPROC g_MainWindowProc = nullptr;
// static WNDPROC g_PrevWindowProc = nullptr;

// // static std::vector<fx_GUILayer*> GuiWindows;
// static std::vector<fx_Widget*> g_Widgets;

void fx_Widget::SetMouseHover(bool Hover)
{
    if (Hover != m_MouseHover)
    {
        m_MouseHover = Hover;
        if (Hover)
        {
            MouseEnterEvent();
        }
        else
        {
            MouseLeaveEvent();
        }
    }
}

void fx_Widget::SetMouseDown(bool Down)
{
    if (Down != m_MouseDown)
    {
        m_MouseDown = Down;
        if (Down)
        {
            MouseDownEvent();
        }
        else
        {
            MouseUpEvent();
        }
    }
}

void fx_Widget::MouseEnterEvent()
{
    m_FlagUpdateMesh = true;
    m_PrevState = m_State;
    if (m_State != disable)
    {
        m_State = hover;
    }
}

void fx_Widget::MouseLeaveEvent()
{
    m_State = m_PrevState;
    m_FlagUpdateMesh = true;
    if (m_State != disable)
    {
        m_State = normal;
    }
}

void fx_Widget::MouseDownEvent()
{
    m_FlagUpdateMesh = true;
    m_PrevState = m_State;
    if (m_State != disable)
    {
        m_State = pressed;
    }
    m_HoldTimer = std::chrono::high_resolution_clock::now();
}

void fx_Widget::MouseUpEvent()
{
    m_FlagUpdateMesh = true;
    if (m_PrevState != disable)
    {
        m_State = m_PrevState;
    }
    
    if (m_MouseHover)
    {
        // {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_HoldTimer) 
        < std::chrono::milliseconds(HOLD_THRESHOLD))
        {
            if (m_MainActionCallback)
            {
                m_MainActionCallback();
            }
        }
        // }
        // else
        // {
        //     if (m_HoldActionCallback)
        //     {
        //         m_HoldActionCallback();
        //     }
        // }
    }
}

void fx_WidgetHandler::Update()
{
    for (auto x : m_Widgets)
    {
        Rect3D ObjRect = x->GetRect();
        if (fx_Collide(ObjRect, m_MousePos))
        {
            x->SetMouseHover(true);
            x->SetMouseDown(m_MouseDown);
            if (x->GetMouseDown())
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - x->GetHoldTImer()) 
                >= std::chrono::milliseconds(HOLD_THRESHOLD))
                {
                    if (x->m_HoldActionCallback)
                    {
                        x->m_HoldActionCallback();
                    }
                }
            }
        }
        else
        {
            x->SetMouseHover(false);
            x->SetMouseDown(false);
        }

        
    }
}

fx_Button::fx_Button(glm::vec3 Pos, glm::vec2 Size, float LineHeight, fx_Font *Font, std::string Text, 
    glm::vec4 NormalColour, glm::vec4 HoverColour, glm::vec4 PressedColour, glm::vec4 DisableColour, glm::vec4 TextColour)
{
    m_TextObj = new fx_Text(Pos + glm::vec3(0.0f,0.0f,1.0f), LineHeight, Font, Text, TextColour);
    m_QuadObj = new fx_Quad(Pos, Size);
    SetPosition(Pos);
    SetCube(glm::vec3(Size, 1.0f));
    SetNormalColour(NormalColour);
    SetHoverColour(HoverColour);
    SetPressedColour(PressedColour);
    SetDisableColour(DisableColour);
    m_QuadObj->SetColour(m_Colours[m_State]);
    m_Objects = {m_TextObj, m_QuadObj};
}

void fx_Button::Update()
{
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    m_Rect.Min = m_Position - (m_Cube * m_Anchor);
    m_Rect.Max = m_Position + (m_Cube * (1.0f - m_Anchor));
    m_Rect.Min.z = m_Position.z;
    m_Rect.Max.z = m_Position.z;
    glm::vec3 CubeCentre = (m_Rect.Min + m_Rect.Max) / 2.0f;
    m_QuadObj->SetColour(m_Colours[m_State]);
    m_QuadObj->SetAnchor(m_Anchor);
    m_TextObj->SetAnchor({0.5f,0.5f,0.0f});
    m_TextObj->SetPosition(CubeCentre + glm::vec3(0.0f,0.0f,1.0f));
    m_QuadObj->SetPosition(m_Position);
    // m_TextObj->m_Position
    m_FlagUpdateObject = false;
    m_FlagUpdateMesh = false;
}


// void SetDPIScale()
// {
//     // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
//     // idk why undefined
// }

// // LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// // {

// // }

// // int64_t hjaooia(void* hwnd, unsigned int uMsg, unsigned int* wParam, int64_t lParam)
// LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// { 
//     switch (uMsg)
//     {
//     case WM_COMMAND:
//     {    
//         fx_Widget *WidgetOwner = nullptr;
//         for (auto &x: g_Widgets)
//         {
//             if ((HWND)x->GetID() == (HWND)lParam)
//             {
//                 WidgetOwner = x;
//                 break;
//             }
//         }

//         switch (HIWORD(wParam))
//         {
//         case BN_CLICKED:
//         {
//             fx_Button *WidgetButton = (fx_Button *)WidgetOwner;
//             if (WidgetButton->m_ClickCallback != nullptr)
//             {
//                 WidgetButton->m_ClickCallback();
//                 return 0;
//             }
//             break;
//         }
//         default:
//             break;
//         }
//         return 0;
//     }

//     default:
//         break;
//     }
//     // return DefWindowProcW(hWnd, uMsg, wParam, lParam);
//     return CallWindowProcW((WNDPROC)g_PrevWindowProc, (HWND)hWnd, uMsg, (WPARAM)wParam, (LPARAM)lParam); 
// } 

// // void Addbutton(GLFWwindow *Window)
// // {
// //     MainWindowProc = (WNDPROC)GetWindowLongPtrW(glfwGetWin32Window(Window), GWLP_WNDPROC);
// //     SetWindowLongPtrW(glfwGetWin32Window(Window), GWLP_WNDPROC, (LONG_PTR)WindowProc);
// //     auto a = WindowProc;
// //     HWND hwndButton = CreateWindowExW( 
// //         0,
// //         L"BUTTON",  // Predefined class; Unicode assumed 
// //         L"OK",      // Button text 
// //         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,  // Styles 
// //         10,         // x position 
// //         10,         // y position 
// //         100,        // Button width
// //         100,        // Button height
// //         glfwGetWin32Window(Window),     // Parent window
// //         NULL,       // No menu.
// //         NULL, 
// //         NULL
// //     );      // Pointer not needed.
// //     if (hwndButton == NULL)
// //     {
// //         std::cout << "can't create button\n";
// //     }
// // }

// fx_GUILayer::fx_GUILayer(void *MainWindow)
// {
//     // m_MainWindowID = glfwGetWin32Window((GLFWwindow *)MainWindow);
//     // WNDCLASSW wc = {0};

//     // wc.style		 = CS_HREDRAW | CS_VREDRAW;
//     // wc.lpfnWndProc   = WindowProc;
//     // wc.lpszClassName = L"GUI Window";
//     // wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
//     // wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

//     // RegisterClassW(&wc);

//     // m_ID = CreateWindowExW(
//     //     WS_EX_LAYERED,
//     //     L"GUI Window",
//     //     L"",
//     //     WS_VISIBLE | WS_CHILD,
//     //     0, 0, 500, 500,
//     //     (HWND)m_MainWindowID,
//     //     NULL,
//     //     NULL,
//     //     NULL
//     // );
//     m_ID = glfwGetWin32Window((GLFWwindow *)MainWindow);

//     // SetLayeredWindowAttributes((HWND)m_ID, RGB(0,0,0), 1, LWA_ALPHA);


//     g_PrevWindowProc = (WNDPROC)GetWindowLongPtrW((HWND)m_ID, GWLP_WNDPROC);
//     SetWindowLongPtrW((HWND)m_ID, GWLP_WNDPROC, (LONG_PTR)WindowProc);
//     // GuiWindows.push_back(this);
// }

// fx_GUILayer::~fx_GUILayer()
// {
//     // GuiWindows.erase(std::find(GuiWindows.begin(),GuiWindows.end(),this));
// }

// // void fx_GUILayer::Update()
// // {
// //     // for (auto &x: m_Widgets)
// //     // {
// //     //     x->Update();
// //     // }
// // }

// glm::ivec2 fx_GUILayer::World2Screen(glm::vec3 Pos)
// {
//     Pos = Pos / m_GameScale;
//     Pos.y *= -1;
//     Pos += glm::vec3({m_GameAspect, 1.0f, 0.0f});
//     Pos *= m_GameSize.y / 2.0f;
//     Pos += glm::vec3({m_GameOffset.x, m_GameOffset.y, 0.0f});
//     return Pos;
// }

// glm::vec3 fx_GUILayer::Screen2World(glm::ivec2 Pos)
// {
//     glm::vec3 Result = glm::vec3({Pos.x, Pos.y, 0});
//     Result -= glm::vec3({m_GameOffset.x, m_GameOffset.y, 0.0f});
//     Result /= m_GameSize.y / 2.0f;
//     Result -= glm::vec3({m_GameAspect, 1.0f, 0.0f});
//     Result.y *= -1;
//     Result = Result * m_GameScale;
//     return Result;
// }


// fx_Button::fx_Button(fx_GUILayer *Window, std::string Text)
// {
//     // m_WidgetHandle = (void*)GetWindowLongPtrW((HWND)Window->GetGUIWindowHndl(), GWLP_WNDPROC);
//     // SetWindowLongPtrW((HWND)Window->GetGUIWindowHndl(), GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
//     m_ID = CreateWindowExA( 
//         // WS_EX_LAYERED ,
//         WS_EX_LAYERED , 
//         "BUTTON",  // Predefined class; Unicode assumed 
//         Text.c_str(),      // Button text 
//         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,  // Styles 
//         0,         // x position 
//         0,         // y position 
//         100,        // Button width
//         100,        // Button height
//         (HWND)Window->GetID(),     // Parent window
//         // NULL, 
//         NULL,       // No menu.
//         NULL, 
//         NULL
//     );      // Pointer not needed.
//     // Window->m_Widgets.push_back(this);

//     m_Complex = false;
//     m_Drawable = false;
//     m_Vertices = {
//         { 0.0f, 1.0f, 0.0f},
//         { 1.0f, 0.0f, 0.0f},
//     };
//     m_Text = Text;
//     m_GUI = Window;
//     Update();
//     g_Widgets.push_back(this);

//     // SetLayeredWindowAttributes((HWND)m_ID, RGB(255,0,0), 255, LWA_ALPHA);
// }

// void fx_Button::Update()
// {
//     m_ModelMatrix = glm::translate(glm::vec3({m_Info.m_Position.x, m_Info.m_Position.y, m_Info.m_Position.z})) * glm::translate(-(m_Info.m_Anchor * m_Info.m_Size)) * glm::scale(m_Info.m_Size);
//     m_ModelVertices.resize(m_Vertices.size());
//     for (unsigned int i = 0; i < m_Vertices.size(); i++)
//     {
//         m_ModelVertices[i] = m_ModelMatrix * glm::vec4(m_Vertices[i],1.0f);
//     }
//     glm::ivec2 Pos = m_GUI->World2Screen(m_ModelVertices[0]);
//     glm::ivec2 Pos2 = m_GUI->World2Screen(m_ModelVertices[1]);
//     glm::ivec2 Size = Pos2-Pos;

//     SetWindowTextA((HWND)m_ID, m_Text.c_str());
//     SetWindowPos((HWND)m_ID, HWND_TOP, Pos.x, Pos.y, Size.x, Size.y, 0);
//     SetLayeredWindowAttributes((HWND)m_ID, RGB(m_Info.m_Color.r*255,m_Info.m_Color.g*255,m_Info.m_Color.b*255), std::max(1,(int)(m_Info.m_Color.a*255)), LWA_ALPHA);

// }

// fx_Message::fx_Message(fx_GUILayer *Window, std::string Title, std::string Caption)
// {
//     m_ID = (void*)MessageBoxA((HWND)Window->GetID(), Caption.c_str(), Title.c_str(),MB_OK);
//     m_Complex = false;
//     m_Drawable = false;
//     m_GUI = Window;
//     // g_Widgets.push_back(this);
// }