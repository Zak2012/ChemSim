#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.hpp"
#include "Resource.hpp"
#include "Physics.hpp"

enum fx_BasicType
{
    Basic = 0,
    Sprite,
    Circle,
    SDF,
};

// add image

class fx_Objects
{
protected:
    virtual void GenerateMesh(){}
    
    bool m_FlagUpdateMesh = true;
    bool m_FlagUpdateObject = true;

    bool m_Enabled = true;
    glm::vec4 m_Colour = {1,1,1,1};
    glm::vec3 m_Cube = {1,1,1};
    glm::vec3 m_Position = {0,0,0};
    glm::vec3 m_Anchor = {0.5f,0.5f,0.0f}; // middle centre
    glm::quat m_Quat = {1.0f, 0.0f, 0.0f, 0.0f};

public:
    // fx_ObjectInfo m_Info;
    bool GetEnable(){return m_Enabled;}
    // bool GetFlagUpdateMesh() {return m_FlagUpdateMesh;}
    // bool GetFlagUpdateObject() {return m_FlagUpdateObject;}
    virtual bool GetComplex(){return false;}
    virtual bool GetDrawable(){return false;}
    virtual glm::vec4 GetColour(){return m_Colour;}
    virtual glm::vec3 GetCube(){return m_Cube;}
    virtual glm::vec3 GetPosition(){return m_Position;}
    virtual glm::vec3 GetAnchor(){return m_Anchor;}
    virtual glm::quat GetQuat(){return m_Quat;}
    virtual bool GetNeedUpdate(){return m_FlagUpdateMesh || m_FlagUpdateObject;}

    void SetEnable(bool Enable){m_FlagUpdateObject = m_Enabled!=Enable; m_Enabled = Enable;}
    virtual void SetColour(glm::vec4 Colour){m_FlagUpdateMesh |= m_Colour!=Colour; m_Colour = Colour;}
    virtual void SetCube(glm::vec3 Cube){m_FlagUpdateMesh |= m_Cube!=Cube; m_Cube = Cube;}
    virtual void SetPosition(glm::vec3 Position){m_FlagUpdateMesh |= m_Position!=Position; m_Position = Position;}
    virtual void SetAnchor(glm::vec3 Anchor){m_FlagUpdateMesh |= m_Anchor!=Anchor; m_Anchor = Anchor;}
    virtual void SetQuat(glm::quat Quat){m_FlagUpdateMesh |= m_Quat!=Quat; m_Quat = Quat;}

    virtual void Update(){}

    friend class fx_Group;
};

class fx_Basic : public fx_Objects
{
protected:
    fx_Mesh m_Mesh = {};
    // fx_Batch *m_Batch = NULL;
    glm::mat4 m_ModelMatrix = glm::identity<glm::mat4>();
    std::vector<glm::vec3> m_ModelVertices;
    glm::vec3 m_Normal = glm::vec4(0,0,1,1);
    glm::vec3 m_Tangent = glm::vec4(1,0,0,1);
    // fx_BasicType m_Type;
    std::vector<glm::vec3> m_Vertices;
    virtual void GenerateMesh() {}
    void Update();
public:
    virtual ~fx_Basic(){}
    virtual bool GetComplex(){return false;}
    virtual bool GetDrawable(){return true;}

    // void SetBatch(fx_Batch *Batch);
    
    // fx_Batch* GetBatch() {return m_Batch;}
    std::vector<glm::vec3> GetModelVertices() {return m_ModelVertices;}
    glm::mat4 GetModelMatrix() {return m_ModelMatrix;}
    fx_Mesh GetMesh();
    virtual fx_BasicType GetType() {return fx_BasicType::Basic;}
    friend class fx_Group;
};

class fx_Triangle : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Colour = {1,1,1,1});
    virtual ~fx_Triangle(){}
    virtual fx_BasicType GetType() {return fx_BasicType::Basic;}
};

class fx_Quad : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour = {1,1,1,1});
    virtual ~fx_Quad(){}
    virtual fx_BasicType GetType() {return fx_BasicType::Basic;}
};

// class fx_Line : public fx_Basic
// {
// protected:
//     void GenerateMesh();
// public:
//     glm::vec3 m_A;
//     glm::vec3 m_B;
//     fx_Line(glm::vec3 Pos1, glm::vec3 Pos2, float Width, glm::vec4 Color = {1,1,1,1});
//     ~fx_Line(){}
//     static fx_Mesh GetBasicMesh();
// };

class fx_Sprite: public fx_Basic
{
protected:
    void GenerateMesh();
    fx_UV m_UV;
    float m_Depth = 0;
public:
    fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour = {1,1,1,1});
    virtual ~fx_Sprite(){}

    float GetDepth(){return m_Depth;}
    fx_UV GetUV(){return m_UV;}

    void SetUV(fx_UV UV){m_FlagUpdateMesh |= (m_UV.X1!=UV.X1)||(m_UV.X2!=UV.X2)||(m_UV.Y1!=UV.Y1)||(m_UV.Y2!=UV.Y2); m_UV = UV;}
    void SetDepth(float Depth){m_FlagUpdateMesh |= m_Depth!=Depth; m_Depth = Depth;};
    virtual fx_BasicType GetType() {return fx_BasicType::Sprite;}
};

class fx_Circle: public fx_Basic
{
protected:
    void GenerateMesh();
    // float m_Angle = 0; //radians
    float m_Depth = 0;
    float m_Outline = 0;
public:
    fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour = {1,1,1,1});
    virtual ~fx_Circle(){}

    float GetDepth(){return m_Depth;}
    // float GetAngle(){return m_Angle;}
    float GetOutline(){return m_Outline;}

    // void SetAngle(float Angle){m_FlagUpdateMesh |= m_Angle!=Angle; m_Angle = Angle;};
    void SetOutline(float Outline){m_FlagUpdateMesh |= m_Outline!=Outline; m_Outline = Outline;};
    void SetDepth(float Depth){m_FlagUpdateMesh |= m_Depth!=Depth; m_Depth = Depth;};
    virtual fx_BasicType GetType() {return fx_BasicType::Circle;}
};

class fx_SDF : public fx_Basic
{
protected:
    void GenerateMesh();
    fx_UV m_UV;
    // glm::vec2 m_GlowThreshold = {0.5f, 0.5f};
    // glm::vec4 m_GlowColour = {1,1,1,1};
    // glm::vec2 m_OutlineThreshold = {0.5f, 0.5f};
    // glm::vec4 m_OutlineColour = {1,1,1,1};
public:
    fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour = {1,1,1,1});
    virtual ~fx_SDF(){}

    fx_UV GetUV(){return m_UV;}
    // glm::vec2 GetGlowThreshold(){return m_GlowThreshold;}
    // glm::vec4 GetGlowColour(){return m_GlowColour;}
    // glm::vec2 GetOutlineThreshold(){return m_OutlineThreshold;}
    // glm::vec4 GetOutlineColour(){return m_OutlineColour;}

    void SetUV(fx_UV UV){m_FlagUpdateMesh |= (m_UV.X1!=UV.X1)||(m_UV.X2!=UV.X2)||(m_UV.Y1!=UV.Y1)||(m_UV.Y2!=UV.Y2); m_UV = UV;}
    // void SetGlowTreshold(glm::vec2 GlowThreshold){m_FlagUpdateMesh |= m_GlowThreshold!=GlowThreshold; m_GlowThreshold = GlowThreshold;}
    // void SetGlowColour(glm::vec4 GlowColour){m_FlagUpdateMesh |= m_GlowColour!=GlowColour; m_GlowColour = GlowColour;}
    // void SetOutlineTreshold(glm::vec2 OutlineTreshold){m_FlagUpdateMesh |= m_OutlineThreshold!=OutlineTreshold; m_OutlineThreshold = OutlineTreshold;}
    // void SetOutlineColour(glm::vec4 OutlineColour){m_FlagUpdateMesh |= m_OutlineColour!=OutlineColour; m_OutlineColour = OutlineColour;}
    virtual fx_BasicType GetType() {return fx_BasicType::SDF;}
};

class fx_Complex : public fx_Objects
{
protected:
    std::vector<fx_Objects *> m_Objects;

public:
    virtual void Update(){}
    virtual bool GetComplex(){return true;}
    virtual bool GetDrawable(){return true;}
    std::vector<fx_Objects *> GetObjects(){return m_Objects;}
};

class fx_Line : public fx_Complex
{
protected:
    fx_Quad *m_Object;
    glm::vec3 m_Start;
    glm::vec3 m_End;
    float m_Height;
public:
    fx_Line(glm::vec3 Start, glm::vec3 End, float Height, glm::vec4 Colour = {1,1,1,1})
    {
        m_Object = new fx_Quad({0.0f,0.0f,0.f}, {0.0f,0.0f}, Colour);
        m_Object->SetAnchor({0.5f,0.5f,0.0f});
        m_Objects = {m_Object};
        SetStart(Start);
        SetEnd(End);
        SetHeight(Height);
    }
    virtual ~fx_Line(){delete m_Object;}

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

class fx_Camera
{
protected:
    glm::mat4 m_LookAtMat = glm::identity<glm::mat4>();
    glm::mat4 m_ProjectionMat = glm::identity<glm::mat4>();
    glm::mat4 m_Mat = glm::identity<glm::mat4>();
    glm::mat4 m_InvMat = glm::identity<glm::mat4>();
    glm::vec3 m_Position = {0,0,0};
    glm::quat m_Quat = {1.0f, 0.0f, 0.0f, 0.0f};
    float m_Size = 0.5f;
    float m_Aspect = 16.0f/9.0f;
    float m_Near = 0.1f;
    float m_Far = 10.0f;

    void UpdateLookAtMat();
    virtual void UpdateProjectionMat(){}
public:

    glm::mat4 GetMat(){return m_Mat;}
    glm::mat4 GetLookAt(){return m_LookAtMat;}
    glm::mat4 GetProjection(){return m_ProjectionMat;}
    glm::vec3 GetPosition(){return m_Position;}
    glm::quat GetQuat(){return m_Quat;}
    float GetSize(){return m_Size;}
    float GetAspect(){return m_Aspect;}
    float GetNear(){return m_Near;}
    float GetFar(){return m_Far;}

    void SetPosition(glm::vec3 Position){m_Position = Position; UpdateLookAtMat();}
    void SetQuat(glm::quat Quat){m_Quat = Quat; UpdateLookAtMat();}
    void SetSize(float Size){m_Size = Size; UpdateProjectionMat();}
    void SetAspect(float Aspect){m_Aspect = Aspect; UpdateProjectionMat();}
    void SetNear(float Near){m_Near = Near; UpdateProjectionMat();}
    void SetFar(float Far){m_Far = Far; UpdateProjectionMat();}

    Line3D Screen2World(glm::vec2 A);
};

class fx_Orthographic : public fx_Camera
{
protected:
    void UpdateProjectionMat();
public:
    fx_Orthographic(glm::vec3 Position, float Aspect){SetPosition(Position); SetAspect(Aspect);};
    ~fx_Orthographic(){};
};

class fx_Perspective : public fx_Camera
{
protected:
    void UpdateProjectionMat();
public:
    fx_Perspective(glm::vec3 Position, float Aspect){SetPosition(Position); SetAspect(Aspect);};
    ~fx_Perspective(){};
};

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
    std::vector<fx_Billboard*> m_Billboard; 
    glm::vec3 m_CameraPos;
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
public:
    glm::vec3 GetCameraPos(){return m_CameraPos;}
    glm::vec3 GetCameraUp(){return m_CameraUp;}

    void SetCameraPos(glm::vec3 CameraPos){m_CameraPos = CameraPos;}
    void SetCameraUp(glm::vec3 CameraUp){m_CameraUp = CameraUp;}

    void AddObject(fx_Billboard *Obj){m_Billboard.push_back(Obj);}
    void DelObject(fx_Billboard *Obj){m_Billboard.erase(std::remove(m_Billboard.begin(), m_Billboard.end(), Obj), m_Billboard.end());}

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

// TODO: also handle image and texture
class fx_Group
{
protected:
    std::vector<std::vector<fx_Basic*>> m_Basics;
    std::vector<fx_Mesh>m_Meshes;
    std::vector<fx_Objects*> m_Objects;
    bool m_FlagUpdateMesh = false;
    bool m_FlagUpdateObject = false;
    fx_Camera* m_Camera = NULL;
    // uint32_t m_ObjCount = 0;
    void GenerateMesh();
    void UpdateDFS(std::vector<fx_Objects*> Objects);
    void CombineBasicDFS(std::vector<std::vector<fx_Basic*>> &Basics, std::vector<fx_Objects*> Objects);
    
public:
    std::vector<fx_Program*> m_Programs;
    std::vector<fx_Buffer*> m_Buffers;
    fx_Texture *m_TextureUnit = NULL;
    fx_Framebuffer *m_FrameBuffer = NULL;
    fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit);

    fx_Camera* GetCamera(){return m_Camera;}

    void Update();
    void Draw();
    void AddObject(fx_Objects *Obj){m_Objects.push_back(Obj); m_FlagUpdateObject = true;}
    void DelObject(fx_Objects *Obj){m_Objects.erase(std::remove(m_Objects.begin(), m_Objects.end(), Obj), m_Objects.end()); m_FlagUpdateObject = true;}
    void SetCamera(fx_Camera *Camera){m_Camera = Camera;}
};

// struct fx_Scene
// {
//     bool Enable = true;
//     std::vector<fx_Group*> Group;
//     std::vector<fx_Framebuffer*> FrameBuffer ;
//     std::vector<std::pair<void*,size_t>>  Heap;
//     std::function<void()> Start = nullptr;
//     std::function<void()> Close = nullptr;

//     void Draw(){for (auto x :  Group){x->Draw();}}
//     void ResetBuffer(){for (auto x :  FrameBuffer){x->ResetBuffer();}}
// };


