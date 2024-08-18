#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Object.hpp"

// Will be absorbed later in windowing/event

void SetDPIScale();

class fx_GUILayer
{
protected:
    void* m_ID;
    // int64_t m_PrevCallback;
    // void* m_MainWindowID;
    // glm::ivec2 m_WindowSize;
public:
    fx_GUILayer(void *MainWindow);
    ~fx_GUILayer();

    // std::vector<fx_Widget *> m_Widgets;
    glm::ivec2 m_GameSize;
    glm::ivec2 m_GameOffset;
    float m_GameScale;
    float m_GameAspect;

    void* GetID() {return m_ID;}
    // int64_t GetPrevCallback() {return m_PrevCallback;}

    glm::ivec2 World2Screen(glm::vec3 Pos);
    glm::vec3 Screen2World(glm::ivec2 Pos);

    

    // float m_GameAspectRatio;

    // void Update();
};

class fx_Widget : public fx_Objects
{
protected:
    void* m_ID;
public:
    fx_ObjectInfo m_Info;

    void* GetID() {return m_ID;}
    // glm::ivec
};

class fx_Button : public fx_Widget
{
protected:
    glm::mat4 m_ModelMatrix = glm::identity<glm::mat4>();
    std::vector<glm::vec3> m_ModelVertices;
    std::vector<glm::vec3> m_Vertices;
    fx_GUILayer *m_GUI;
public:
    fx_Button(fx_GUILayer *Window, std::string Text);
    ~fx_Button(){};

    
    // SetParent()

    void Update();
    std::string m_Text = "";

    std::function<void()> m_ClickCallback = nullptr;
};

class fx_Message : public fx_Widget
{
protected:
    fx_GUILayer *m_GUI;
public:
    fx_Message(fx_GUILayer *Window, std::string Title, std::string Caption);
    ~fx_Message(){};
};

// void Addbutton(GLFWwindow *Window);