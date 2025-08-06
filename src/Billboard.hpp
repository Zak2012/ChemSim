#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Object.hpp"

class fx_Billboard : public fx_Complex
{
protected:
    glm::vec3 m_CameraPos;
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
public:
    glm::vec3 GetCameraPos(){return m_CameraPos;}
    glm::vec3 GetCameraUp(){return m_CameraUp;}

    virtual void SetCameraPos(glm::vec3 CameraPos){m_FlagUpdateMesh |= m_CameraPos!=CameraPos; m_CameraPos = CameraPos;}
    virtual void SetCameraUp(glm::vec3 CameraUp){m_FlagUpdateMesh |= m_CameraUp!=CameraUp; m_CameraUp = CameraUp;}

    virtual void Update(){}
};

class fx_BillboardHandler
{
protected:
    std::set<fx_Billboard*> m_Billboard; 
    glm::vec3 m_CameraPos;
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
public:
    glm::vec3 GetCameraPos(){return m_CameraPos;}
    glm::vec3 GetCameraUp(){return m_CameraUp;}

    void SetCameraPos(glm::vec3 CameraPos){m_CameraPos = CameraPos;}
    void SetCameraUp(glm::vec3 CameraUp){m_CameraUp = CameraUp;}

    void AddObject(fx_Billboard *Obj){m_Billboard.insert(Obj);}
    void DelObject(fx_Billboard *Obj){m_Billboard.erase(Obj);}

    virtual void Update(){for(auto x:m_Billboard){x->SetCameraPos(m_CameraPos); x->SetCameraUp(m_CameraUp);}}
};

class fx_BillboardCircle : public fx_Billboard
{
protected:
    fx_Circle *m_Object;
public:
    fx_BillboardCircle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour = {1,1,1,1})
    {
        m_Object = new fx_Circle(Pos, Size, Colour);
        m_Objects = {m_Object};
    }
    virtual ~fx_BillboardCircle(){delete m_Object;}

    // glm::vec4 GetColour(){return m_Object->GetColour();}
    // glm::vec3 GetCube(){return m_Object->GetCube();}
    // glm::vec3 GetPosition(){return m_Object->GetPosition();}
    // glm::vec3 GetAnchor(){return m_Object->GetAnchor();}
    float GetOutline(){return m_Object->GetOutline();}

    // void SetColour(glm::vec4 Colour){m_Object->SetColour(Colour);}
    // void SetCube(glm::vec3 Cube){m_Object->SetCube(Cube);}
    // void SetPosition(glm::vec3 Position){m_Object->SetPosition(Position);}
    // void SetAnchor(glm::vec3 Anchor){m_Object->SetAnchor(Anchor);}
    void SetOutline(float Outline){m_Object->SetOutline(Outline);};


    void Update();
    
};

class fx_BillboardLine : public fx_Billboard
{
protected:
    fx_Sprite *m_Object;
    glm::vec3 m_Start;
    glm::vec3 m_End;
    float m_Height;
public:
    fx_BillboardLine(glm::vec3 Start, glm::vec3 End, float Height, glm::vec4 Colour = {1,1,1,1})
    {
        fx_UV uv;
        uv.X1 = 0;
        uv.X2 = 0;
        uv.Y1 = 1;
        uv.Y2 = -1;
        m_Object = new fx_Sprite({0.0f,0.0f,0.f}, {0.0f,0.0f}, uv, Colour);
        m_Object->SetAnchor({0.5f,0.5f,0.0f});
        m_Objects = {m_Object};
        SetStart(Start);
        SetEnd(End);
        SetHeight(Height);
    }
    virtual ~fx_BillboardLine(){delete m_Object;}

    // glm::vec4 GetColour(){return m_Object->GetColour();}
    glm::vec3 GetCube(){return m_Object->GetCube();}
    glm::vec3 GetPosition(){return m_Object->GetPosition();}
    glm::vec3 GetAnchor(){return m_Object->GetAnchor();}
    glm::vec3 GetStart(){return m_Start;}
    glm::vec3 GetEnd(){return m_End;}
    float GetHeight(){return m_Height;}


    // void SetColour(glm::vec4 Colour){m_Object->SetColour(Colour);}
    void SetStart(glm::vec3 Start){m_FlagUpdateMesh |= m_Start!=Start; m_Start = Start;}
    void SetEnd(glm::vec3 End){m_FlagUpdateMesh |= m_End!=End; m_End = End;}
    void SetHeight(float Height){m_FlagUpdateMesh |= m_Height!=Height; m_Height = Height;}
    
    void Update();

};