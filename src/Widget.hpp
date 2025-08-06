#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Object.hpp"
#include "Font.hpp"


class fx_Widget : public fx_Complex
{
protected:
    bool m_MouseDown = false;
    bool m_WidgetEnable = true;
    Rect3D m_Rect;
    Line3D m_MousePos;

    #ifndef __EMSCRIPTEN__
    std::chrono::system_clock::time_point m_HoldTimer;
    #else
    std::chrono::steady_clock::time_point m_HoldTimer;
    #endif

    virtual void MouseDownEvent();
    virtual void MouseUpEvent();

public:

    // bool GetMouseHover(){return m_MouseHover;}
    bool GetMouseDown(){return m_MouseDown;}
    Rect3D GetRect(){return m_Rect;}
    // fx_WidgetState GetWidgetState(){return m_State;}
    bool GetWidgetEnable(){return m_WidgetEnable;}

    #ifndef __EMSCRIPTEN__
    std::chrono::system_clock::time_point GetHoldTImer(){return m_HoldTimer;}
    #else
    std::chrono::steady_clock::time_point GetHoldTImer(){return m_HoldTimer;}
    #endif

    // void SetMouseHover(bool Hover);
    void SetMouseDown(bool Down);
    void SetWidgetEnable(bool Enable){m_FlagUpdateMesh |= m_WidgetEnable!=Enable; m_WidgetEnable = Enable;};
    void SetMousePos(Line3D MousePos){m_FlagUpdateMesh |= m_MousePos!=MousePos; m_MousePos = MousePos;}

    std::function<void()> m_MainActionCallback = nullptr;
    std::function<void()> m_HoldActionCallback = nullptr;



    virtual void Update(){}
};

class fx_WidgetHandler
{
protected:
    std::set<fx_Widget*> m_Widgets; 
    Line3D m_MousePos;
    bool m_MouseDown = false;
public:
    fx_WidgetHandler(){}
    ~fx_WidgetHandler(){}

    bool GetMouseDown(){return m_MouseDown;}
    Line3D GetMousePos(){return m_MousePos;}

    void SetMouseDown(bool Down){m_MouseDown = Down;}
    void SetMousePos(Line3D MousePos){m_MousePos = MousePos;}
    
    void AddObject(fx_Widget *Obj){m_Widgets.insert(Obj);}
    void DelObject(fx_Widget *Obj){m_Widgets.erase(Obj);}
    void Update();
};

class fx_Button : public fx_Widget
{
protected:
    // fx_Text* m_TextObj;
    fx_Quad* m_QuadObj;
    fx_Sprite* m_SpriteObj;
    glm::vec4 m_ColourDisable;
    glm::vec4 m_ColourDefault;
    glm::vec4 m_ColourPresses;
    // glm::vec4 m_Colours[4];
public:
    fx_Button(glm::vec3 Pos, glm::vec2 Size, fx_UV uv, 
        glm::vec4 NormalColour, glm::vec4 PressedColour, glm::vec4 DisableColour, glm::vec4 TextColour = {1,1,1,1});
    ~fx_Button(){ delete m_SpriteObj; delete m_QuadObj; }

    glm::vec4 GetNormalColour(){return m_ColourDefault;}
    glm::vec4 GetPressedColour(){return m_ColourPresses;}
    glm::vec4 GetDisableColour(){return m_ColourDisable;}
    // glm::vec4 GetTextColour(){return m_TextObj->GetColour();}
    glm::vec4 GetSpriteColour(){return m_SpriteObj->GetColour();}
    // fx_Font *GetFont(){return m_TextObj->GetFont();}
    // std::string GetText(){return m_TextObj->GetText();}
    fx_UV GetUV(){return m_SpriteObj->GetUV();}
    // float GetLineHeight(){return m_TextObj->GetLineHeight();}

    void SetNormalColour(glm::vec4 NormalColour){m_FlagUpdateMesh |= m_ColourDefault!=NormalColour; m_ColourDefault = NormalColour;}
    void SetPressedColour(glm::vec4 PressedColour){m_FlagUpdateMesh |= m_ColourPresses!=PressedColour; m_ColourPresses = PressedColour;}
    void SetDisableColour(glm::vec4 DisableColour){m_FlagUpdateMesh |= m_ColourDisable!=DisableColour; m_ColourDisable = DisableColour;}
    void SetSpriteColour(glm::vec4 TextColour){m_SpriteObj->SetColour(TextColour);}
    // void SetTextColour(glm::vec4 TextColour){m_TextObj->SetColour(TextColour);}
    // void SetFont(fx_Font *Font){m_TextObj->SetFont(Font);}
    // void SetText(std::string Text){m_TextObj->SetText(Text);}
    void SetUV(fx_UV UV){m_SpriteObj->SetUV(UV);}
    // void SetLineHeight(float LineHeight){m_TextObj->SetLineHeight(LineHeight);}

    void Update();
};


// #undef WINVER
// #define WINVER NTDDI_WIN10_19H1

// #undef _WIN32_WINNT
// #define _WIN32_WINNT _WIN32_WINNT_WIN10

// // Will be absorbed later in windowing/event

// void SetDPIScale();

// class fx_GUILayer
// {
// protected:
//     void* m_ID;
//     // int64_t m_PrevCallback;
//     // void* m_MainWindowID;
//     // glm::ivec2 m_WindowSize;
// public:
//     fx_GUILayer(void *MainWindow);
//     ~fx_GUILayer();

//     // std::vector<fx_Widget *> m_Widgets;
//     glm::ivec2 m_GameSize;
//     glm::ivec2 m_GameOffset;
//     float m_GameScale;
//     float m_GameAspect;

//     void* GetID() {return m_ID;}
//     // int64_t GetPrevCallback() {return m_PrevCallback;}

//     glm::ivec2 World2Screen(glm::vec3 Pos);
//     glm::vec3 Screen2World(glm::ivec2 Pos);

    

//     // float m_GameAspectRatio;

//     // void Update();
// };

// class fx_Widget : public fx_Objects
// {
// protected:
//     void* m_ID;
// public:
//     // fx_ObjectInfo m_Info;

//     void* GetID() {return m_ID;}
//     // glm::ivec
// };

// class fx_Button : public fx_Widget
// {
// protected:
//     glm::mat4 m_ModelMatrix = glm::identity<glm::mat4>();
//     std::vector<glm::vec3> m_ModelVertices;
//     std::vector<glm::vec3> m_Vertices;
//     fx_GUILayer *m_GUI;
// public:
//     fx_Button(fx_GUILayer *Window, std::string Text);
//     ~fx_Button(){};

    
//     // SetParent()

//     void Update();
//     std::string m_Text = "";

//     std::function<void()> m_ClickCallback = nullptr;
// };

// class fx_Message : public fx_Widget
// {
// protected:
//     fx_GUILayer *m_GUI;
// public:
//     fx_Message(fx_GUILayer *Window, std::string Title, std::string Caption);
//     ~fx_Message(){};
// };

// // void Addbutton(GLFWwindow *Window);