#pragma once

#include <vector>
#include <string>
#include <unordered_map>
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
    bool m_Complex;
    bool m_Drawable;
    virtual void GenerateMesh(){}
    
    bool m_FlagUpdateMesh = true;
    bool m_FlagUpdateObject = true;

    bool m_Enabled = true;
    glm::vec4 m_Colour = {1,1,1,1};
    glm::vec3 m_Cube = {1,1,1};
    glm::vec3 m_Position = {0,0,0};
    glm::vec3 m_Anchor = {0,0,0}; // bottom left
    glm::quat m_Quat = {1.0f, 0.0f, 0.0f, 0.0f};

public:
    // fx_ObjectInfo m_Info;
    bool GetEnable(){return m_Enabled;}
    virtual glm::vec4 GetColour(){return m_Colour;}
    virtual glm::vec3 GetCube(){return m_Cube;}
    virtual glm::vec3 GetPosition(){return m_Position;}
    virtual glm::vec3 GetAnchor(){return m_Anchor;}
    virtual glm::quat GetQuat(){return m_Quat;}

    void SetEnable(bool Enable){m_FlagUpdateObject = m_Enabled!=Enable; m_Enabled = Enable;}
    bool GetComplex(){return m_Complex;}
    bool GetDrawable(){return m_Drawable;}
    virtual void SetColour(glm::vec4 Colour){m_FlagUpdateMesh |= m_Colour!=Colour; m_Colour = Colour;}
    virtual void SetCube(glm::vec3 Cube){m_FlagUpdateMesh |= m_Cube!=Cube; m_Cube = Cube;}
    virtual void SetPosition(glm::vec3 Position){m_FlagUpdateMesh |= m_Position!=Position; m_Position = Position;}
    virtual void SetAnchor(glm::vec3 Anchor){m_FlagUpdateMesh |= m_Anchor!=Anchor; m_Anchor = Anchor;}
    virtual void SetQuat(glm::quat Quat){m_FlagUpdateMesh |= m_Quat!=Quat; m_Quat = Quat;}

    ~fx_Objects(){}
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
    fx_BasicType m_Type;
    std::vector<glm::vec3> m_Vertices;
    virtual void GenerateMesh() {}
    void Update();
public:
    ~fx_Basic(){}

    // void SetBatch(fx_Batch *Batch);
    
    // fx_Batch* GetBatch() {return m_Batch;}
    std::vector<glm::vec3> GetModelVertices() {return m_ModelVertices;}
    glm::mat4 GetModelMatrix() {return m_ModelMatrix;}
    fx_Mesh GetMesh();
    fx_BasicType GetType() {return m_Type;}
    friend class fx_Group;
};

class fx_Triangle : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Colour = {1,1,1,1});
    ~fx_Triangle(){}
};

class fx_Quad : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour = {1,1,1,1});
    ~fx_Quad(){}
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
public:
    fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour = {1,1,1,1});
    ~fx_Sprite(){}

    fx_UV GetUV(){return m_UV;}

    void SetUV(fx_UV UV){m_FlagUpdateMesh |= (m_UV.X1!=UV.X1)||(m_UV.X2!=UV.X2)||(m_UV.Y1!=UV.Y1)||(m_UV.Y2!=UV.Y2); m_UV = UV;}
};

class fx_Circle: public fx_Basic
{
protected:
    void GenerateMesh();
    float m_Angle = 0; //radians
    float m_Outline = 0;
public:
    fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour = {1,1,1,1});
    ~fx_Circle(){}

    float GetAngle(){return m_Angle;}
    float GetOutline(){return m_Outline;}

    void SetAngle(float Angle);
    void SetOutline(float Outline);
};

class fx_SDF : public fx_Basic
{
protected:
    void GenerateMesh();
    fx_UV m_UV;
    glm::vec2 m_GlowThreshold = {0.5f, 0.5f};
    glm::vec4 m_GlowColour = {1,1,1,1};
    glm::vec2 m_OutlineThreshold = {0.5f, 0.5f};
    glm::vec4 m_OutlineColour = {1,1,1,1};
public:
    fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour = {1,1,1,1});
    ~fx_SDF(){}

    fx_UV GetUV(){return m_UV;}
    glm::vec2 GetGlowThreshold(){return m_GlowThreshold;}
    glm::vec4 GetGlowColour(){return m_GlowColour;}
    glm::vec2 GetOutlineThreshold(){return m_OutlineThreshold;}
    glm::vec4 GetOutlineColour(){return m_OutlineColour;}

    void SetUV(fx_UV UV){m_FlagUpdateMesh |= (m_UV.X1!=UV.X1)||(m_UV.X2!=UV.X2)||(m_UV.Y1!=UV.Y1)||(m_UV.Y2!=UV.Y2); m_UV = UV;}
    void SetGlowTreshold(glm::vec2 GlowThreshold){m_FlagUpdateMesh |= m_GlowThreshold!=GlowThreshold; m_GlowThreshold = GlowThreshold;}
    void SetGlowColour(glm::vec4 GlowColour){m_FlagUpdateMesh |= m_GlowColour!=GlowColour; m_GlowColour = GlowColour;}
    void SetOutlineTreshold(glm::vec2 OutlineTreshold){m_FlagUpdateMesh |= m_OutlineThreshold!=OutlineTreshold; m_OutlineThreshold = OutlineTreshold;}
    void SetOutlineColour(glm::vec4 OutlineColour){m_FlagUpdateMesh |= m_OutlineColour!=OutlineColour; m_OutlineColour = OutlineColour;}
};

class fx_Complex : public fx_Objects
{
protected:
    std::vector<fx_Objects *> m_Objects;

    // void GenerateMesh();

    // friend 
public:
    // glm::vec3 m_Scale = {0.0f,0.0f,0.0f};
    virtual void Update(){}
    std::vector<fx_Objects *> GetObjects(){return m_Objects;}
};

class fx_Camera
{
protected:
    glm::mat4 m_LookAtMat = glm::identity<glm::mat4>();
    glm::mat4 m_ProjectionMat = glm::identity<glm::mat4>();
    glm::mat4 m_Mat = glm::identity<glm::mat4>();
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

//also handle image and texture
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
    
public:
    std::vector<fx_Program*> m_Programs;
    std::vector<fx_Buffer*> m_Buffers;
    fx_Texture *m_TextureUnit;
    fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit);

    fx_Camera* GetCamera(){return m_Camera;}

    void Update();
    void Draw();
    void AddObject(fx_Objects *Obj){m_Objects.push_back(Obj); m_FlagUpdateObject = true;}
    void DelObject(fx_Objects *Obj){m_Objects.erase(std::remove(m_Objects.begin(), m_Objects.end(), Obj), m_Objects.end()); m_FlagUpdateObject = true;}
    void SetCamera(fx_Camera *Camera){m_Camera = Camera;}
};

class fx_Framebuffer
{
protected:
    glm::ivec2 m_Size = {100,100};
    fx_Texture *m_ColorAttachment;
    unsigned int m_Framebuffer;
    unsigned int m_StencilAttachment;
public:
    fx_Framebuffer(bool Linear = false);
    ~fx_Framebuffer();
    fx_Texture *GetColorAttachment(){return m_ColorAttachment;}
    unsigned int GetFramebuffer(){return m_Framebuffer;}
    unsigned int GetStencilAttachment(){return m_StencilAttachment;}
    void SetSize(glm::ivec2 Size);
    glm::ivec2 GetSize(){return m_Size;}
    std::vector<fx_Group *> Groups;
    void Bind();
    void Unbind();
};
