#include "Widget.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>

#include <iostream>
#include <vector>
#include <string>
#include <iostream>

// static WNDPROC g_MainWindowProc = nullptr;
static WNDPROC g_PrevWindowProc = nullptr;

// static std::vector<fx_GUILayer*> GuiWindows;
static std::vector<fx_Widget*> g_Widgets;

// LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {

// }

// int64_t hjaooia(void* hwnd, unsigned int uMsg, unsigned int* wParam, int64_t lParam)
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    switch (uMsg)
    {
    case WM_COMMAND:
    {    
        fx_Widget *WidgetOwner = nullptr;
        for (auto &x: g_Widgets)
        {
            if ((HWND)x->GetID() == (HWND)lParam)
            {
                WidgetOwner = x;
                break;
            }
        }

        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            fx_Button *WidgetButton = (fx_Button *)WidgetOwner;
            if (WidgetButton->m_ClickCallback != nullptr)
            {
                WidgetButton->m_ClickCallback();
                return 0;
            }
            break;
        }
        default:
            break;
        }
        return 0;
    }

    default:
        break;
    }
    // return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    return CallWindowProcW((WNDPROC)g_PrevWindowProc, (HWND)hWnd, uMsg, (WPARAM)wParam, (LPARAM)lParam); 
} 

// void Addbutton(GLFWwindow *Window)
// {
//     MainWindowProc = (WNDPROC)GetWindowLongPtrW(glfwGetWin32Window(Window), GWLP_WNDPROC);
//     SetWindowLongPtrW(glfwGetWin32Window(Window), GWLP_WNDPROC, (LONG_PTR)WindowProc);
//     auto a = WindowProc;
//     HWND hwndButton = CreateWindowExW( 
//         0,
//         L"BUTTON",  // Predefined class; Unicode assumed 
//         L"OK",      // Button text 
//         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,  // Styles 
//         10,         // x position 
//         10,         // y position 
//         100,        // Button width
//         100,        // Button height
//         glfwGetWin32Window(Window),     // Parent window
//         NULL,       // No menu.
//         NULL, 
//         NULL
//     );      // Pointer not needed.
//     if (hwndButton == NULL)
//     {
//         std::cout << "can't create button\n";
//     }
// }

fx_GUILayer::fx_GUILayer(void *MainWindow)
{
    // m_MainWindowID = glfwGetWin32Window((GLFWwindow *)MainWindow);
    // WNDCLASSW wc = {0};

    // wc.style		 = CS_HREDRAW | CS_VREDRAW;
    // wc.lpfnWndProc   = WindowProc;
    // wc.lpszClassName = L"GUI Window";
    // wc.hCursor       = LoadCursorA(NULL, IDC_ARROW);
    // wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

    // RegisterClassW(&wc);

    // m_ID = CreateWindowExW(
    //     WS_EX_LAYERED,
    //     L"GUI Window",
    //     L"",
    //     WS_VISIBLE | WS_CHILD,
    //     0, 0, 500, 500,
    //     (HWND)m_MainWindowID,
    //     NULL,
    //     NULL,
    //     NULL
    // );
    m_ID = glfwGetWin32Window((GLFWwindow *)MainWindow);

    // SetLayeredWindowAttributes((HWND)m_ID, RGB(0,0,0), 1, LWA_ALPHA);


    g_PrevWindowProc = (WNDPROC)GetWindowLongPtrW((HWND)m_ID, GWLP_WNDPROC);
    SetWindowLongPtrW((HWND)m_ID, GWLP_WNDPROC, (LONG_PTR)WindowProc);
    // GuiWindows.push_back(this);
}

fx_GUILayer::~fx_GUILayer()
{
    // GuiWindows.erase(std::find(GuiWindows.begin(),GuiWindows.end(),this));
}

// void fx_GUILayer::Update()
// {
//     // for (auto &x: m_Widgets)
//     // {
//     //     x->Update();
//     // }
// }

glm::ivec2 fx_GUILayer::World2Screen(glm::vec3 Pos)
{
    Pos.y *= -1;
    Pos += glm::vec3({m_GameAspect, 1.0f, 0.0f});
    Pos *= m_GameSize.y / 2.0f;
    Pos += glm::vec3({m_GameOffset.x, m_GameOffset.y, 0.0f});
    return Pos;
}

glm::vec3 fx_GUILayer::Screen2World(glm::ivec2 Pos)
{
    glm::vec3 Result = glm::vec3({Pos.x, Pos.y, 0});
    Result -= glm::vec3({m_GameOffset.x, m_GameOffset.y, 0.0f});
    Result /= m_GameSize.y / 2.0f;
    Result -= glm::vec3({m_GameAspect, 1.0f, 0.0f});
    Result.y *= -1;
    return Result;
}


fx_Button::fx_Button(fx_GUILayer *Window)
{
    // m_WidgetHandle = (void*)GetWindowLongPtrW((HWND)Window->GetGUIWindowHndl(), GWLP_WNDPROC);
    // SetWindowLongPtrW((HWND)Window->GetGUIWindowHndl(), GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    m_ID = CreateWindowExW( 
        // WS_EX_LAYERED ,
        WS_EX_LAYERED , 
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"OK",      // Button text 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,  // Styles 
        0,         // x position 
        0,         // y position 
        100,        // Button width
        100,        // Button height
        (HWND)Window->GetID(),     // Parent window
        // NULL, 
        NULL,       // No menu.
        NULL, 
        NULL
    );      // Pointer not needed.
    // Window->m_Widgets.push_back(this);

    m_Complex = false;
    m_Drawable = false;
    m_Vertices = {
        { 0.0f, 1.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
    };
    m_GUI = Window;
    Update();
    g_Widgets.push_back(this);

    // SetLayeredWindowAttributes((HWND)m_ID, RGB(255,0,0), 255, LWA_ALPHA);
}

void fx_Button::Update()
{
    m_ModelMatrix = glm::translate(glm::vec3({m_Info.m_Position.x, m_Info.m_Position.y, m_Info.m_Position.z})) * glm::translate(-(m_Info.m_Anchor * m_Info.m_Size)) * glm::scale(m_Info.m_Size);
    m_ModelVertices.resize(m_Vertices.size());
    for (unsigned int i = 0; i < m_Vertices.size(); i++)
    {
        m_ModelVertices[i] = m_ModelMatrix * glm::vec4(m_Vertices[i],1.0f);
    }
    glm::ivec2 Pos = m_GUI->World2Screen(m_ModelVertices[0]);
    glm::ivec2 Pos2 = m_GUI->World2Screen(m_ModelVertices[1]);
    glm::ivec2 Size = Pos2-Pos;

    SetWindowTextA((HWND)m_ID, m_Text.c_str());
    SetWindowPos((HWND)m_ID, HWND_TOP, Pos.x, Pos.y, Size.x, Size.y, 0);
    SetLayeredWindowAttributes((HWND)m_ID, RGB(m_Info.m_Color.r*255,m_Info.m_Color.g*255,m_Info.m_Color.b*255), std::max(1,(int)(m_Info.m_Color.a*255)), LWA_ALPHA);

}