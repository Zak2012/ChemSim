#include "Widget.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <string>

#define HOLD_THRESHOLD 500

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

void fx_Widget::MouseDownEvent()
{
    m_FlagUpdateMesh = true;
    m_HoldTimer = std::chrono::high_resolution_clock::now();
}

void fx_Widget::MouseUpEvent()
{
    m_FlagUpdateMesh = true;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_HoldTimer) 
    < std::chrono::milliseconds(HOLD_THRESHOLD))
    {
        if (m_MainActionCallback && m_WidgetEnable)
        {
            m_MainActionCallback();
        }
    }
}

void fx_WidgetHandler::Update()
{
    for (auto x : m_Widgets)
    {
        if (!x->GetWidgetEnable())
        {
            continue;
        }
        x->SetMousePos(m_MousePos);
        if (fx_Collide(x->GetRect(), m_MousePos))
        {
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
            x->SetMouseDown(false);
        }
    }
}

fx_Button::fx_Button(glm::vec3 Pos, glm::vec2 Size, float LineHeight, fx_Font *Font, std::string Text, 
    glm::vec4 NormalColour, glm::vec4 PressedColour, glm::vec4 DisableColour, glm::vec4 TextColour)
{
    m_TextObj = new fx_Text(Pos + glm::vec3(0.0f,0.0f,1.0f), LineHeight, Font, Text, TextColour);
    m_QuadObj = new fx_Quad(Pos, Size);
    SetPosition(Pos);
    SetCube(glm::vec3(Size, 1.0f));
    SetNormalColour(NormalColour);
    SetPressedColour(PressedColour);
    SetDisableColour(DisableColour);
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
    if (m_WidgetEnable)
    {
        if (m_MouseDown)
        {
            m_QuadObj->SetColour(m_ColourPresses);
        }
        else
        {
            m_QuadObj->SetColour(m_ColourDefault);
        }
    }
    else
    {
        m_QuadObj->SetColour(m_ColourDisable);
    }
    // m_QuadObj->SetColour(m_Colours[m_State]);
    m_QuadObj->SetAnchor(m_Anchor);
    m_TextObj->SetAnchor({0.5f,0.5f,0.0f});
    m_TextObj->SetPosition(CubeCentre + glm::vec3(0.0f,0.0f,1.0f));
    m_QuadObj->SetPosition(m_Position);
    // m_TextObj->m_Position
    m_FlagUpdateObject = false;
    m_FlagUpdateMesh = false;
}