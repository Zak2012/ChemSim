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
    Basic,
    Sprite,
    Circle,
    SDF,
};

std::unordered_map<fx_BasicType, fx_Buffer*> fx_InitBufferMap();

struct fx_ObjectInfo
{
    glm::vec4 m_Color = {1,1,1,1};
    glm::vec3 m_Position = {0,0,0};
    glm::vec3 m_Anchor = {0,0,0}; // bottom left
    glm::vec3 m_Size = {1,1,1};
    glm::quat m_Rotation = {1.0f, 0.0f, 0.0f, 0.0f};
};

class fx_Objects
{
protected:
    bool m_Complex;
    bool m_Drawable;
    // std::vector<fx_Objects *> m_Objects;
    virtual void GenerateMesh(){}
public:
    bool m_Enabled = true;
    fx_ObjectInfo m_Info;
    // glm::vec3 m_Size = {0.0f,0.0f,0.0f};
    // Rect3D m_BoundingBox = {{0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f}};
    // std::vector<glm::vec3> m_CollisionPoints = {{0.0f,0.0f,1.0f},{1.0f,0.0f,1.0f},{1.0f,1.0f,1.0f},{0.0f,1.0f,1.0f}};
    // Poly3D m_Poly;
    virtual void Update(){}
    bool GetComplex(){return m_Complex;}
    bool GetDrawable(){return m_Drawable;}
    ~fx_Objects()
    {
        // for (auto &x: m_Objects)
        // {
        //     delete x;
        // }
        // m_Objects.clear();
    }
    // std::vector<fx_Objects *> GetObjects(){return m_Objects;}
};

class fx_Basic : public fx_Objects
{
protected:
    fx_Mesh m_Mesh = {};
    fx_Batch *m_Batch = NULL;
    glm::mat4 m_ModelMatrix = glm::identity<glm::mat4>();
    std::vector<glm::vec3> m_ModelVertices;
    fx_BasicType m_Type;
    std::vector<glm::vec3> m_Vertices;
    virtual void GenerateMesh() {}
public:
    ~fx_Basic(){}

    void Update();
    void SetBatch(fx_Batch *Batch);
    
    fx_Batch* GetBatch() {return m_Batch;}
    std::vector<glm::vec3> GetModelVertices() {return m_ModelVertices;}
    glm::mat4 GetModelMatrix() {return m_ModelMatrix;}
    fx_Mesh GetMesh();
    fx_BasicType GetType() {return m_Type;}
};

class fx_Triangle : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Color = {1,1,1,1});
    ~fx_Triangle(){}
    static fx_Mesh GetBasicMesh();
};

class fx_Quad : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Color = {1,1,1,1});
    ~fx_Quad(){}
    static fx_Mesh GetBasicMesh();
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
public:
    fx_UV m_UV;
    fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color = {1,1,1,1});
    ~fx_Sprite(){}
    static fx_Mesh GetBasicMesh();
};

class fx_Circle: public fx_Basic
{
protected:
    void GenerateMesh();
public:
    float m_Angle = 0; //radians
    float m_Outline = 0;
    fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Color = {1,1,1,1});
    ~fx_Circle(){}
    static fx_Mesh GetBasicMesh();
};

class fx_SDF : public fx_Basic
{
protected:
    void GenerateMesh();
public:
    glm::vec2 m_GlowThreshold = {0.5f, 0.5f};
    glm::vec4 m_GlowColor = {1,1,1,1};
    glm::vec2 m_OutlineThreshold = {0.5f, 0.5f};
    glm::vec4 m_OutlineColor = {1,1,1,1};
    fx_UV m_UV;
    fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color = {1,1,1,1});
    ~fx_SDF(){}
    static fx_Mesh GetBasicMesh();
};

class fx_Complex : public fx_Objects
{
protected:
    std::vector<fx_Objects *> m_Objects;

    // void GenerateMesh();

    // friend 
public:
    // glm::vec3 m_Scale = {0.0f,0.0f,0.0f};
    virtual void Update();
    std::vector<fx_Objects *> GetObjects(){return m_Objects;}
};

//https://easings.net
enum fx_InterpolationType
{
    Linear,
    Hold,
    SmoothStep,
    Bezier,
    EaseIn, //default ease is Sine
    EaseOut,
    EaseInOut,

    EaseInSine,
    EaseOutSine,
    EaseInOutSine,

    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,

    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,

    EaseInQuart,
    EaseOutQuart,
    EaseInOutQuart,

    EaseInQuint,
    EaseOutQuint,
    EaseInOutQuint,

    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,

    EaseInCirc,
    EaseOutCirc,
    EaseInOutCirc,

    EaseInBack,
    EaseOutBack,
    EaseInOutBack,

    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,

    EaseInBounce,
    EaseOutBounce,
    EaseInOutBounce,
};

struct fx_Keyframe
{
    fx_ObjectInfo Start;
    fx_ObjectInfo End;
    fx_InterpolationType InterType;
    std::vector<glm::vec2> Bezier = {{0,0},{0,0}};
};


class fx_Animator
{
private:
public:
    fx_ObjectInfo Interpolate(float Pos, fx_Keyframe Key);
    float m_Duration = 1;
    float m_Pos = 0.0;
    fx_Keyframe m_KeyFrame;
    fx_ObjectInfo m_Result;
    void Animate()
    {
        m_Result = Interpolate(m_Pos, m_KeyFrame);
    }
    void Advance(float Step)
    {
        m_Pos += Step/(m_Duration<=0.0f?1.0f:m_Duration);
        m_Pos = std::min(m_Pos, 1.0f);
        m_Pos = std::max(m_Pos, 0.0f);
    }
};

class fx_Group
{
protected:
    std::unordered_map<fx_BasicType, fx_Batch*> m_Batches;
public:
    std::vector<fx_Objects*> m_Objects;
    std::unordered_map<fx_BasicType, fx_Program*> m_Programs;
    std::unordered_map<fx_BasicType, fx_Buffer*> m_Buffers;
    fx_Texture *m_TextureUnit;
    fx_Group(std::unordered_map<fx_BasicType, fx_Program*> Programs, std::unordered_map<fx_BasicType, fx_Buffer*> Buffers, fx_Texture *TextureUnit);
    void GenerateMesh();
    void Update();
    void Draw();
};

// struct fx_Scene
// {
//     void (*SetupFunction)() = NULL;
//     void (*UpdateFunction)(float) = NULL;
//     void (*ExitFunction)() = NULL;
//     // std::vector<fx_Group *> Groups;
//     // std::vector<fx_Framebuffer *> FrameBuffer;
// };


class fx_Framebuffer
{
private:
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
