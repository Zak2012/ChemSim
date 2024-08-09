#include "Object.hpp"

#define GLEW_STATIC

#include <stdexcept>
#include <exception>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <algorithm>

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.hpp"
#include "Resource.hpp"

#define CALCULATEMESH() {\
    m_Mesh = GetBasicMesh();\
    unsigned int VertexSize = 0;\
    for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)\
    {\
        VertexSize += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;\
    }\
    m_Mesh.Vertices = std::vector<unsigned char>((unsigned char *)(Vertices.data()), (unsigned char *)(Vertices.data()) + (VertexSize * Vertices.size()));\
}\

std::unordered_map<fx_BasicType, fx_Buffer*> fx_InitBufferMap()
{
    std::unordered_map<fx_BasicType, fx_Buffer*> Buffer;
    Buffer[fx_BasicType::Basic] = new fx_Buffer(fx_Quad::GetBasicMesh());
    Buffer[fx_BasicType::Sprite] = new fx_Buffer(fx_Sprite::GetBasicMesh());
    Buffer[fx_BasicType::Circle] = new fx_Buffer(fx_Circle::GetBasicMesh());
    Buffer[fx_BasicType::SDF] = new fx_Buffer(fx_SDF::GetBasicMesh());
    return Buffer;
}

void fx_Basic::Update()
{
    m_ModelMatrix = glm::translate(m_Info.m_Position) * glm::toMat4(m_Info.m_Rotation) * glm::translate(-(m_Info.m_Anchor * m_Info.m_Size)) * glm::scale(m_Info.m_Size);

    m_ModelVertices.resize(m_Vertices.size());
    for (unsigned int i = 0; i < m_Vertices.size(); i++)
    {
        m_ModelVertices[i] = m_ModelMatrix * glm::vec4(m_Vertices[i],1.0f);
    }

    // m_Poly.Corners.resize(m_CollisionPoints.size());
    // for (unsigned int i = 0; i < m_Poly.Corners.size(); i++)
    // {
    //     glm::vec4 Vertex = m_ModelMatrix * glm::vec4(m_CollisionPoints[i],1.0f);
    //     m_Poly.Corners[i] = glm::vec3(Vertex.x, Vertex.y, Vertex.z);
    // }

    // std::vector<glm::vec3> SortedPos = m_Poly.Corners;
    // std::sort(std::begin(SortedPos), std::end(SortedPos),
    //       [] (const auto& lhs, const auto& rhs) {
    //     return lhs.x < rhs.x;
    //     });
    // m_BoundingBox.Min.x = SortedPos.front().x;
    // m_BoundingBox.Max.x = SortedPos.back().x;

    // std::sort(std::begin(SortedPos), std::end(SortedPos),
    //       [] (const auto& lhs, const auto& rhs) {
    //     return lhs.y < rhs.y;
    //     });
    // m_BoundingBox.Min.y = SortedPos.front().y;
    // m_BoundingBox.Max.y = SortedPos.back().y;

    // std::sort(std::begin(SortedPos), std::end(SortedPos),
    //       [] (const auto& lhs, const auto& rhs) {
    //     return lhs.z < rhs.z;
    //     });
    // m_BoundingBox.Min.z = SortedPos.front().z;
    // m_BoundingBox.Max.z = SortedPos.back().z;
    GenerateMesh(); 
}

fx_Mesh fx_Basic::GetMesh()
{
    if (m_Enabled && m_Drawable)
    {
        return m_Mesh;
    }
    return fx_Mesh({});
}

void fx_Basic::SetBatch(fx_Batch *Batch)
{
    m_Batch = Batch; 
    m_Batch->Add(&m_Mesh);
}

fx_Triangle::fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Color)
{
    // m_Objects = {this};
    m_Info.m_Position = Pos;
    m_Info.m_Size = glm::vec3(Size,1);
    m_Vertices = Vertices;
    m_Info.m_Color = Color;
    m_Complex = false;
    m_Drawable = true;
    m_Type = fx_BasicType::Basic;
    Update();
}

fx_Mesh fx_Triangle::GetBasicMesh()
{
    return {{},{},{3,4},
        {{GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}}};
}

void fx_Triangle::GenerateMesh()
{
    struct __attribute__((__packed__)) fx_m_Mesh
    {
        glm::vec3 pos;
        glm::vec4 col;
    };
    std::vector<fx_m_Mesh> Vertices = {{},{},{}};
    for (unsigned int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].pos = m_ModelVertices[i];
        Vertices[i].col = m_Info.m_Color;
    }
    CALCULATEMESH();
    m_Mesh.Indices = {0,1,2};
}

fx_Quad::fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Color)
{
    m_Info.m_Position = Pos;
    m_Info.m_Color = Color;
    m_Complex = false;
    // m_Objects = {this};
    m_Drawable = true;
    m_Info.m_Size = glm::vec3(Size,1);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
    m_Type = fx_BasicType::Basic;
    Update();
}

fx_Mesh fx_Quad::GetBasicMesh()
{
    return {{},{},{3,4},
        {{GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}}};
}

void fx_Quad::GenerateMesh()
{
    struct __attribute__((__packed__)) fx_m_Mesh
    {
        glm::vec3 pos;
        glm::vec4 col;
    };
    std::vector<fx_m_Mesh> Vertices = {{},{},{},{}};
    for (unsigned int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].pos = m_ModelVertices[i];
        Vertices[i].col = m_Info.m_Color;
    }
    CALCULATEMESH();
    m_Mesh.Indices = {0,1,2, 0,2,3};
}

fx_Sprite::fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color)
{
    m_Info.m_Position = Pos;
    m_Drawable = true;
    m_Complex = false;
    // m_Objects = {this};
    m_Info.m_Size = glm::vec3(Size,1);
    m_Info.m_Color = Color;
    m_UV = UV;
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
    m_Type = fx_BasicType::Sprite;
    Update();
}

fx_Mesh fx_Sprite::GetBasicMesh()
{
    return {{},{},{3,4,2},
        {{GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}}};
}

void fx_Sprite::GenerateMesh()
{
    struct __attribute__((__packed__)) fx_m_Mesh
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec2 uv;
    };
    std::vector<fx_m_Mesh> Vertices = {{},{},{},{}};
    for (unsigned int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].pos = m_ModelVertices[i];
        Vertices[i].col = m_Info.m_Color;
    }
    Vertices[0].uv = {m_UV.X1, m_UV.Y2};
    Vertices[1].uv = {m_UV.X2, m_UV.Y2};
    Vertices[2].uv = {m_UV.X2, m_UV.Y1};
    Vertices[3].uv = {m_UV.X1, m_UV.Y1};
    CALCULATEMESH();
    m_Mesh.Indices = {0,1,2, 0,2,3};
}

fx_Circle::fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Color)
{
    m_Info.m_Position = Pos;
    m_Info.m_Color = Color;
    m_Drawable = true;
    m_Complex = false;
    // m_Objects = {this};
    m_Info.m_Size = glm::vec3(Size,1);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
    m_Type = fx_BasicType::Circle;
    Update();
}

fx_Mesh fx_Circle::GetBasicMesh()
{
    return {{},{},{3,4,2,1,1},
        {{GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}}};
}

void fx_Circle::GenerateMesh()
{
    struct __attribute__((__packed__)) fx_m_Mesh
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec2 uv;
        float Outline;
        float Angle;
    };
    std::vector<fx_m_Mesh> Vertices = {{},{},{},{}};
    for (unsigned int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].pos = m_ModelVertices[i];
        Vertices[i].col = m_Info.m_Color;
        Vertices[i].Outline = m_Outline;
        Vertices[i].Angle = m_Angle;
    }
    Vertices[0].uv = {-1, -1};
    Vertices[1].uv = { 1, -1};
    Vertices[2].uv = { 1,  1};
    Vertices[3].uv = {-1,  1};
    CALCULATEMESH();
    m_Mesh.Indices = {0,1,2, 0,2,3};
}

fx_SDF::fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color)
{
    m_Info.m_Position = Pos;
    m_Drawable = true;
    m_Complex = false;
    // m_Objects = {this};
    m_Info.m_Size = glm::vec3(Size,1);
    m_Info.m_Color = Color;
    m_UV = UV;
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
    m_Type = fx_BasicType::SDF;
    Update();
}

fx_Mesh fx_SDF::GetBasicMesh()
{
    return {{},{},{3,4,2,2,4,2,4},
        {{GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)},
        {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}}};
}

void fx_SDF::GenerateMesh()
{
    struct __attribute__((__packed__)) fx_m_Mesh
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec2 uv;
        glm::vec2 Gt;
        glm::vec4 Gcol;
        glm::vec2 Ot;
        glm::vec4 OCol;
    };
    std::vector<fx_m_Mesh> Vertices = {{},{},{},{}};
    for (unsigned int i = 0; i < Vertices.size(); i++)
    {
        Vertices[i].pos = m_ModelVertices[i];
        Vertices[i].col = m_Info.m_Color;
        Vertices[i].Gt = m_GlowThreshold;
        Vertices[i].Gcol = m_GlowColor;
        Vertices[i].Ot = m_OutlineThreshold;
        Vertices[i].OCol = m_OutlineColor;
    }
    Vertices[0].uv = {m_UV.X1, m_UV.Y2};
    Vertices[1].uv = {m_UV.X2, m_UV.Y2};
    Vertices[2].uv = {m_UV.X2, m_UV.Y1};
    Vertices[3].uv = {m_UV.X1, m_UV.Y1};
    CALCULATEMESH();
    m_Mesh.Indices = {0,1,2, 0,2,3};
}

void fx_Complex::Update() 
{
    // UpdateContent();
    for (auto x : m_Objects)
    {
        x->Update();
    }
    // GenerateMesh();
}

// void fx_Complex::GenerateMesh()
// {
    // for (auto &x: m_Objects)
    // {
    //     x->GenerateMesh();
    // }
// }

template<typename T>
T fx_Lerp(T x, T y, float t) {
  return x * (1.f - t) + y * t;
}

float fx_EaseOutBounce(float x)
{
    const float N1 = 7.5625f;
    const float D1 = 2.75f;
    if (x < (1.0f / D1))
    {
        return N1 * x * x;
    }
    else if (x < (2.0f / D1))
    {
        return N1 * (x -= (1.5f / D1)) * x + 0.75f;
    }
    else if (x < (2.5f / D1))
    {
        return N1 * (x -= (2.25f / D1)) * x + 0.9375;
    }
    else
    {
        return N1 * (x -= (2.625 / D1)) * x + 0.984375;
    }
}

fx_ObjectInfo fx_LerpObj(fx_ObjectInfo x, fx_ObjectInfo y, float t)
{
    fx_ObjectInfo Result;
    Result.m_Anchor = fx_Lerp(x.m_Anchor, y.m_Anchor, t);
    Result.m_Color = fx_Lerp(x.m_Color, y.m_Color, t);
    Result.m_Position = fx_Lerp(x.m_Position, y.m_Position, t);
    Result.m_Size = fx_Lerp(x.m_Size, y.m_Size, t);
    Result.m_Rotation = glm::slerp(x.m_Rotation, y.m_Rotation, t);
    return Result;
}

fx_ObjectInfo fx_Animator::Interpolate(float Pos, fx_Keyframe Key)
{
    const float C1 = 1.70158f;
    const float C2 = C1 * 1.525f;
    const float C3 = C1 + 1.0f;
    const float C4 = (2.0f * glm::pi<float>()) / 3.0f;
    const float C5 = (2.0f * glm::pi<float>()) / 4.5f;
    fx_ObjectInfo Result;
    Key.Bezier.resize(2);

    if (Key.InterType == fx_InterpolationType::Hold)
    {
        if (Pos >= 0.5f)
        {
            return Key.End;
        }
        else
        {
            return Key.Start;
        }
    }
    else if (Key.InterType == fx_InterpolationType::Linear)
    {
        return fx_LerpObj(Key.Start, Key.End, Pos);
    }
    else if (Key.InterType == fx_InterpolationType::SmoothStep)
    {
        return fx_LerpObj(Key.Start, Key.End, glm::smoothstep(0.0f, 1.0f, Pos));
    }
    else if (Key.InterType == fx_InterpolationType::Bezier)
    {
        glm::vec2 StartBezier = {0.0f,0.0f};
        glm::vec2 EndBezier = {1.0f,1.0f};
        float AntiT = 1.0f - Pos;
        float y = ((AntiT * AntiT * AntiT * StartBezier) + (3.0f * Pos * AntiT * AntiT * Key.Bezier[0]) + (3.0f * Pos * Pos * AntiT * Key.Bezier[1]) + (Pos * Pos * Pos * EndBezier)).y;
        return fx_LerpObj(Key.Start, Key.End, y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseIn || Key.InterType == fx_InterpolationType::EaseInSine)
    {
        return fx_LerpObj(Key.Start, Key.End, std::cos((Pos * glm::pi<float>()) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOut || Key.InterType == fx_InterpolationType::EaseOutSine)
    {
        return fx_LerpObj(Key.Start, Key.End, std::sin((Pos * glm::pi<float>()) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOut || Key.InterType == fx_InterpolationType::EaseInOutSine)
    {
        return fx_LerpObj(Key.Start, Key.End, -(std::cos(Pos * glm::pi<float>()) - 1.0f) / 2.0f);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInQuad)
    {
        return fx_LerpObj(Key.Start, Key.End, Pos * Pos);
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutQuad)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - ((1.0f-Pos) * (1.0f-Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutQuad)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos < 0.5f)?2.0f * Pos * Pos:1-((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInCubic)
    {
        return fx_LerpObj(Key.Start, Key.End, Pos * Pos * Pos);
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutCubic)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - ((1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutCubic)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos < 0.5f)?4.0f * Pos * Pos * Pos:1.0f-((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInQuart)
    {
        return fx_LerpObj(Key.Start, Key.End, Pos * Pos * Pos * Pos);
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutQuart)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - ((1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutQuart)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos < 0.5f)?8.0f * Pos * Pos * Pos * Pos:1.0f-((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInQuint)
    {
        return fx_LerpObj(Key.Start, Key.End, Pos * Pos * Pos * Pos * Pos);
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutQuint)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - ((1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos) * (1.0f-Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutQuint)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos < 0.5f)?16.0f * Pos * Pos * Pos * Pos * Pos:1.0f-((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f) / 2.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInExpo)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos == 0.0f)?0.0f:std::pow(2.0f, 10.0f * Pos - 10.0f));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutExpo)
    {
        return fx_LerpObj(Key.Start, Key.End, (Pos == 1.0f)?1.0f:(1.0f - std::pow(2.0f, -10.0f * Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutExpo)
    {
        float Y;
        if (Pos == 0.0f || Pos == 1.0f)
        {
            Y = Pos;
        }
        else if (Pos < 0.5)
        {
            Y = std::pow(2.0f, 20.0f * Pos - 10.0f) / 2.0f;
        }
        else
        {
            Y = (2.0f - std::pow(2.0f, -20.0f * Pos + 10.0f)) / 2.0f;
        }
        return fx_LerpObj(Key.Start, Key.End, Y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInCirc)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - std::sqrt(1.0f - (Pos * Pos)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutCirc)
    {
        return fx_LerpObj(Key.Start, Key.End, std::sqrt(1.0f - ((Pos - 1.0f) * (Pos - 1.0f))));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutCirc)
    {
        float Y;
        if (Pos < 0.5f)
        {
            Y = (1.0f - std::sqrt(1.0f - ((2 * Pos) * (2 * Pos)))) / 2.0f;
        }
        else
        {
            Y = (std::sqrt(1.0f - ((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f))) + 1.0f) / 2.0f;
        }
        return fx_LerpObj(Key.Start, Key.End, Y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInBack)
    {
        return fx_LerpObj(Key.Start, Key.End, (C3 * Pos * Pos * Pos) - (C1 * Pos * Pos));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutBack)
    {
        return fx_LerpObj(Key.Start, Key.End,  1.0f + C3 * ((Pos - 1.0f) * (Pos - 1.0f) * (Pos - 1.0f)) + C1 * ((Pos - 1.0f) * (Pos - 1.0f)));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutBack)
    {
        float Y;
        if (Pos < 0.5f)
        {
            Y = (((2.0f * Pos) * (2.0f * Pos)) * ((C2 + 1.0f) * 2.0f * Pos - C2)) / 2.0f;
        }
        else
        {
            Y = (((-2.0f * Pos + 2.0f) * (-2.0f * Pos + 2.0f)) * ((C2 + 1.0f) * (Pos * 2.0f - 2.0f) + C2) + 2.0f) / 2.0f;
        }
        return fx_LerpObj(Key.Start, Key.End, Y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInElastic)
    {
        float Y;
        if (Pos == 0.0f || Pos == 1.0f)
        {
            Y = Pos;
        }
        else
        {
            Y = -std::pow(2.0f, 10.0f * Pos - 10.0f) * std::sin((Pos * 10.0f - 10.75f) * C4);
        }
        return fx_LerpObj(Key.Start, Key.End, (C3 * Y * Y * Y) - (C1 * Y * Y));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutElastic)
    {
        float Y;
        if (Pos == 0.0f || Pos == 1.0f)
        {
            Y = Pos;
        }
        else
        {
            Y = std::pow(2.0f, -10.0f * Pos) * std::sin((Pos * 10.0f - 0.75f) * C4) + 1.0f;
        }
        return fx_LerpObj(Key.Start, Key.End,  Y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutElastic)
    {
        float Y;
        if (Pos == 0.0f || Pos == 1.0f)
        {
            Y = Pos;
        }
        else if (Pos < 0.5f)
        {
            Y = -(std::pow(2.0f, 20.0f * Pos - 10.0f) * std::sin((20.0f * Pos - 11.125f) * C5)) / 2.0f;
        }
        else
        {
            Y = -(std::pow(2.0f, 20.0f * Pos + 10.0f) * std::sin((20.0f * Pos - 11.125f) * C5)) / 2.0f + 1.0f;
        }
        return fx_LerpObj(Key.Start, Key.End, Y);
    }
    else if (Key.InterType == fx_InterpolationType::EaseInBounce)
    {
        return fx_LerpObj(Key.Start, Key.End, 1.0f - fx_EaseOutBounce(1.0f - Pos));
    }
    else if (Key.InterType == fx_InterpolationType::EaseOutBounce)
    {
        return fx_LerpObj(Key.Start, Key.End, fx_EaseOutBounce(Pos));
    }
    else if (Key.InterType == fx_InterpolationType::EaseInOutBounce)
    {
        float Y;
        if (Pos < 0.5f)
        {
            Y = (1.0f - fx_EaseOutBounce(1.0f - 2.0f * Pos)) / 2.0f;
        }
        else
        {
            Y = (1.0f + fx_EaseOutBounce(2.0f * Pos - 1.0f)) / 2.0f;
        }
        return fx_LerpObj(Key.Start, Key.End, Y);
    }


    return Result;

}


fx_Group::fx_Group(std::unordered_map<fx_BasicType, fx_Program*> Programs, std::unordered_map<fx_BasicType, fx_Buffer*> Buffers, fx_Texture *TextureUnit)
{
    m_Programs = Programs;
    m_Buffers = Buffers;
    m_TextureUnit = TextureUnit;
    for (auto x :  m_Buffers)
    {
        m_Batches[x.first] = new fx_Batch();
    }
}

void dfs(std::unordered_map<fx_BasicType, fx_Batch *> Bacthes, std::vector<fx_Objects*> Objects)
{
    for (auto &x: Objects)
    {
        if (!x->GetDrawable())
        {
            continue;
        }
        if (!x->m_Enabled)
        {
            continue;
        }
        if (!x->GetComplex())
        {
            fx_Basic *Basic = (fx_Basic*)x;
            Basic->SetBatch(Bacthes[Basic->GetType()]);
            // std::cout << Bacthes[Basic->GetType()]->GetMesh().Vertices.size() << "\n";
        }
        else
        {
            dfs(Bacthes, ((fx_Complex*)x)->GetObjects());
        }
    }
    return;
}


void fx_Group::GenerateMesh()
{
    for (auto x :  m_Buffers)
    {
        delete m_Batches[x.first];
        m_Batches[x.first] = new fx_Batch();
    }

    dfs(m_Batches, m_Objects);

    for (auto &x : m_Batches)
    {
        x.second->GenerateMesh();
    }
}

void fx_Group::Update()
{
    GenerateMesh();
    for (auto x : m_Batches)
    {
        m_Buffers[x.first]->Update(x.second->GetMesh());
    }
}

void fx_Group::Draw()
{
    std::vector<fx_BasicType> Keys;
    Keys.reserve(m_Batches.size());
    
    for (auto x : m_Batches)
    {
        Keys.push_back(x.first);
    }

    if (m_TextureUnit != NULL)
    {
        m_TextureUnit->Bind();
    }

    for (auto x : Keys)
    {
        if (m_Buffers[x]->GetMeshesIndicesCount() > 0)
        {
            m_Programs[x]->Bind();
            m_Buffers[x]->Bind();
            glDrawElements(GL_TRIANGLES, m_Buffers[x]->GetMeshesIndicesCount(), GL_UNSIGNED_INT, 0);
            m_Programs[x]->Unbind();
            m_Buffers[x]->Unbind();
        }
    }
    
    if (m_TextureUnit != NULL)
    {
        m_TextureUnit->Unbind();
    }
}


fx_Framebuffer::fx_Framebuffer(bool Linear)
{
    fx_Image Image;
    Image.Component = 4;
    Image.Width = m_Size.x;
    Image.Height = m_Size.y;
    m_ColorAttachment = new fx_Texture(Image, Linear);

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment->GetID(), 0);

    glGenRenderbuffers(1, &m_StencilAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, m_StencilAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_ColorAttachment->GetData().Width, m_ColorAttachment->GetData().Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_StencilAttachment);
    GLenum FrameBufferCompleteness = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( FrameBufferCompleteness != GL_FRAMEBUFFER_COMPLETE )
    {
        throw std::runtime_error("Framebuffer hasn't complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

fx_Framebuffer::~fx_Framebuffer()
{
    glDeleteFramebuffers(1, &m_Framebuffer);
    glDeleteRenderbuffers(1, &m_StencilAttachment);
    delete m_ColorAttachment;
}

void fx_Framebuffer::SetSize(glm::ivec2 Size)
{
    if (m_Size != Size)
    {
        m_Size = Size;
        fx_Image Image;
        Image.Component = 4;
        Image.Width = m_Size.x;
        Image.Height = m_Size.y;
        m_ColorAttachment->Update(Image);

        glBindRenderbuffer(GL_RENDERBUFFER, m_StencilAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

void fx_Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}

void fx_Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

