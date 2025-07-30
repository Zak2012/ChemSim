#include "Object.hpp"

#include <stdexcept>
#include <exception>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iostream>

#include <chrono>
#include <thread>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.hpp"
#include "Resource.hpp"

union fx_Basic_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
            glm::vec3 nor;
            glm::vec3 tan;
        } v;
        uint8_t raw[sizeof(v)];
    };

union fx_Sprite_Mesh
{
    struct
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec3 nor;
        glm::vec3 tan;
        glm::vec2 uv;
        float Depth;
    }v;
    uint8_t raw[sizeof(v)];
};


union fx_Circle_Mesh
{
    struct
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec3 nor;
        glm::vec3 tan;
        glm::vec2 uv;
        float Outline;
        // float Angle;
        float Depth;
    }v;
    uint8_t raw[sizeof(v)];
};
    
union fx_SDF_Mesh
{
    struct
    {
        glm::vec3 pos;
        glm::vec4 col;
        glm::vec3 nor;
        glm::vec3 tan;
        glm::vec2 uv;

        // glm::vec2 Gt;
        // glm::vec4 Gcol;
        // glm::vec2 Ot;
        // glm::vec4 OCol;
    }v;
    uint8_t raw[sizeof(v)];
};

fx_Mesh fx_Objects::BasicMeshGenerator(fx_BasicType Type)
{
    fx_Mesh Mesh;
    switch (Type)
    {
    case fx_BasicType::Basic:
        Mesh.VertexComp = {3,4,3,3};
        break;
    case fx_BasicType::Sprite:
        Mesh.VertexComp = {3,4,3,3,2,1};
        break;
    case fx_BasicType::Circle:
        Mesh.VertexComp = {3,4,3,3,2,1,1};
        break;
    case fx_BasicType::SDF:
        Mesh.VertexComp = {3,4,3,3,2};
        break;
    
    default:
        break;
    }

    for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    return Mesh;
}

//slow need fix later
void fx_Basic::Update()
{
    // auto StartTime = std::chrono::high_resolution_clock::now();

    m_ModelMatrix = glm::translate(m_Position) * glm::toMat4(m_Quat) * glm::translate(-(m_Anchor * m_Cube)) * glm::scale(m_Cube);
    
    m_ModelVertices.resize(m_Vertices.size());
    for (unsigned int i = 0; i < m_Vertices.size(); i++)
    {
        m_ModelVertices[i] = m_ModelMatrix * glm::vec4(m_Vertices[i],1.0f);
    }
    
    m_Normal = glm::normalize(glm::cross(m_ModelVertices[1] - m_ModelVertices[0], m_ModelVertices[2] - m_ModelVertices[0]));
    m_Tangent = glm::normalize(m_ModelVertices[1] - m_ModelVertices[0]);

    // auto ATime = std::chrono::high_resolution_clock::now();


    GenerateMesh(); 
    // auto BTime = std::chrono::high_resolution_clock::now();

    // int At = std::chrono::duration_cast<std::chrono::microseconds>(ATime - StartTime).count();
    // int Bt = std::chrono::duration_cast<std::chrono::microseconds>(BTime - ATime).count();

    // std::cout << At << ", " << Bt << "\n";
}

fx_Mesh fx_Basic::GetMesh()
{
    if (GetEnable() && GetDrawable())
    {
        return m_Mesh;
    }
    return fx_Mesh({});
}

void fx_Basic::Draw()
{
    if (m_Program)
    {   
        if (!m_Buffer)
        {
            m_Buffer = new fx_Buffer(BasicMeshGenerator(GetType()));
        }

        if (m_TextureUnit)
        {
            m_TextureUnit->Bind();
        }
        if (m_Buffer)
        {
            m_Buffer->Update(m_Mesh);
            m_Program->SetUniform(m_DrawMat, "Matrix");
            
            if (m_Buffer->GetMeshesIndicesCount() > 0)
            {
                m_Program->Bind();
                m_Buffer->Bind();
                glDrawElements(GL_TRIANGLES, m_Buffer->GetMeshesIndicesCount(), GL_UNSIGNED_INT, 0);
                m_Program->Unbind();
                m_Buffer->Unbind();
            }
            
        }
        if (m_TextureUnit != NULL)
        {
            m_TextureUnit->Unbind();
        }
    }
}

fx_Triangle::fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = Vertices;
}

void fx_Triangle::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(GetType());
    m_Mesh.Indices = {0,1,2};
    m_Mesh.Vertices.reserve(sizeof(fx_Basic_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_Basic_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.nor = m_Normal;
        temp.v.tan = m_Tangent;
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_Basic_Mesh::raw)]);
    }
}

fx_Quad::fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
}

void fx_Quad::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(GetType());
    m_Mesh.Indices = {0,1,2, 0,2,3};
    m_Mesh.Vertices.reserve(sizeof(fx_Basic_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_Basic_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.nor = m_Normal;
        temp.v.tan = m_Tangent;
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_Basic_Mesh::raw)]);
    }
}

fx_Sprite::fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    SetUV(UV);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
}

void fx_Sprite::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(GetType());
    m_Mesh.Indices = {0,1,2, 0,2,3};
    m_Mesh.Vertices.reserve(sizeof(fx_Sprite_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_Sprite_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.nor = m_Normal;
        temp.v.tan = m_Tangent;
        temp.v.Depth = m_Depth;


        if ( i == 0 ){temp.v.uv =      {m_UV.X1, m_UV.Y2};}
        else if ( i == 1 ){temp.v.uv = {m_UV.X2, m_UV.Y2};}
        else if ( i == 2 ){temp.v.uv = {m_UV.X2, m_UV.Y1};}
        else if ( i == 3 ){temp.v.uv = {m_UV.X1, m_UV.Y1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_Sprite_Mesh::raw)]);
    }
}

fx_Circle::fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
}

void fx_Circle::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(GetType());
    m_Mesh.Indices = {0,1,2, 0,2,3};
    m_Mesh.Vertices.reserve(sizeof(fx_Circle_Mesh::raw) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_Circle_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.Outline = m_Outline;
        // temp.v.Angle = m_Angle;
        temp.v.Depth = m_Depth;
        temp.v.nor = m_Normal;
        temp.v.tan = m_Tangent;

        if ( i == 0 ){temp.v.uv =      {-1, -1};}
        else if ( i == 1 ){temp.v.uv = { 1, -1};}
        else if ( i == 2 ){temp.v.uv = { 1,  1};}
        else if ( i == 3 ){temp.v.uv = {-1,  1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_Circle_Mesh::raw)]);
    }
}

fx_SDF::fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = {
        { 0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 0.0f},
        { 1.0f, 1.0f, 0.0f},
        { 0.0f, 1.0f, 0.0f}
        };
    Update();
}

void fx_SDF::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(GetType());
    m_Mesh.Indices = {0,1,2, 0,2,3};
    m_Mesh.Vertices.reserve(sizeof(fx_SDF_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_SDF_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        // temp.v.Gt = m_GlowThreshold;
        // temp.v.Gcol = m_GlowColour;
        // temp.v.Ot = m_OutlineThreshold;
        // temp.v.OCol = m_OutlineColour;
        temp.v.nor = m_Normal;
        temp.v.tan = m_Tangent;

        if ( i == 0 ){temp.v.uv =      {m_UV.X1, m_UV.Y2};}
        else if ( i == 1 ){temp.v.uv = {m_UV.X2, m_UV.Y2};}
        else if ( i == 2 ){temp.v.uv = {m_UV.X2, m_UV.Y1};}
        else if ( i == 3 ){temp.v.uv = {m_UV.X1, m_UV.Y1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_SDF_Mesh::raw)]);
    }
}

void fx_Line::Update()
{
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    glm::vec3 MidPoint = (m_Start + m_End)/2.0f;
    
    m_Object->SetPosition(MidPoint);
    m_Object->SetAnchor({0.5,0.5,0.0});
    m_Object->SetCube({glm::distance(m_Start,m_End),m_Height,m_Object->GetCube().z});

    m_Object->SetColour(m_Colour);
}

void fx_Camera::UpdateLookAtMat()
{
    m_LookAtMat = glm::lookAt(m_Position, m_Position + (glm::vec3(0.0f, 0.0f, -1.0f) * m_Quat), glm::vec3(0.0f, 1.0f,  0.0f) * m_Quat);
    m_Mat = m_ProjectionMat * m_LookAtMat;
    m_InvMat = glm::inverse(m_Mat);
}

Line3D fx_Camera::Screen2World(glm::vec2 A)
{
    Line3D Result;
    glm::vec4 Pos;
    Pos = m_InvMat * glm::vec4(A, -1.0f, 1.0f);
    Result.Start = glm::vec3(Pos) * (1.0f / Pos.w);
    Pos = m_InvMat * glm::vec4(A, 1.0f, 1.0f);
    Result.End = glm::vec3(Pos) * (1.0f / Pos.w);
    return Result;
}


void fx_Orthographic::UpdateProjectionMat()
{
    m_ProjectionMat = glm::ortho( -m_Aspect * m_Size, m_Aspect * m_Size , -1.0f * m_Size, 1.0f * m_Size, m_Near, m_Far );
    m_Mat = m_ProjectionMat * m_LookAtMat;
    m_InvMat = glm::inverse(m_Mat);
}

void fx_Perspective::UpdateProjectionMat()
{
    m_ProjectionMat = glm::perspective(glm::radians(90.0f), m_Aspect, m_Near, m_Far);
    m_Mat = m_ProjectionMat * m_LookAtMat;
    m_InvMat = glm::inverse(m_Mat);
}