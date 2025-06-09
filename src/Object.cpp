#include "Object.hpp"

#define GLEW_STATIC

#include <stdexcept>
#include <exception>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
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

fx_Mesh BasicMeshGenerator(fx_BasicType Type)
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

void fx_Basic::Update()
{
    if (!(m_FlagUpdateMesh || m_FlagUpdateObject))
    {
        return;
    }
    if (!m_Enabled)
    {
        return;
    }
    m_ModelMatrix = glm::translate(m_Position) * glm::toMat4(m_Quat) * glm::translate(-(m_Anchor * m_Cube)) * glm::scale(m_Cube);
    
    m_ModelVertices.resize(m_Vertices.size());
    for (unsigned int i = 0; i < m_Vertices.size(); i++)
    {
        m_ModelVertices[i] = m_ModelMatrix * glm::vec4(m_Vertices[i],1.0f);
    }
    
    m_Normal = glm::normalize(glm::cross(m_ModelVertices[1] - m_ModelVertices[0], m_ModelVertices[2] - m_ModelVertices[0]));
    m_Tangent = glm::normalize(m_ModelVertices[1] - m_ModelVertices[0]);

    GenerateMesh(); 
}

fx_Mesh fx_Basic::GetMesh()
{
    if (GetEnable() && GetDrawable())
    {
        return m_Mesh;
    }
    return fx_Mesh({});
}

fx_Triangle::fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = Vertices;
    m_Type = fx_BasicType::Basic;
}

void fx_Triangle::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(m_Type);
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
    m_Type = fx_BasicType::Basic;
}

void fx_Quad::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(m_Type);
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
    m_Type = fx_BasicType::Sprite;
}

void fx_Sprite::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(m_Type);
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
    m_Type = fx_BasicType::Circle;
}

void fx_Circle::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(m_Type);
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
    m_Type = fx_BasicType::SDF;
    Update();
}

void fx_SDF::GenerateMesh()
{
    m_Mesh = BasicMeshGenerator(m_Type);
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

void fx_BillboardLine::Update()
{
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    glm::vec3 MidPoint = (m_Start + m_End)/2.0f;
    
    glm::vec3 CamObjNormal = glm::normalize(glm::cross(m_CameraPos - m_Start, m_CameraPos - m_End));
    
    glm::vec3 LineVec = m_End-m_Start;
    
    glm::vec3 Front;
    Front = glm::normalize(glm::cross(CamObjNormal, LineVec));
    
    if(glm::dot(Front, glm::normalize(m_CameraPos - MidPoint)) >= std::cos(glm::pi<float>()/2.0f))
    {
        Front = -Front;
    }
    
    m_Object->SetDepth((GetCube().x / 2) * std::abs(Front.z));
    
    
    m_Object->SetPosition(MidPoint);
    m_Object->SetAnchor({0.5,0.5,0.0});
    m_Object->SetCube({glm::distance(m_Start,m_End),m_Height,m_Object->GetCube().z});

    // std::cout << Front.x << "," << Front.y << "," << Front.z << "," << glm::dot(Front, glm::normalize(m_CameraPos - m_Start)) << "\n";
    m_Object->SetQuat(glm::quatLookAt(Front, CamObjNormal));
}

fx_Group::fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit)
{
    m_Programs = Programs;
    m_Buffers.resize(m_Programs.size());
    m_Basics.resize(m_Programs.size());
    m_Meshes.resize(m_Programs.size());
    m_TextureUnit = TextureUnit;
    {
        fx_Mesh Mesh = {{},{},{3,4}, {}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Basic] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{},{3,4,2}, {}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Sprite] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{},{3,4,2,1,1},{}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Circle] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{},{3,4,2,2,4,2,4,},{}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::SDF] = new fx_Buffer(Mesh);
    }
}

void fx_Group::CombineBasicDFS(std::vector<std::vector<fx_Basic*>> &Basics, std::vector<fx_Objects*> Objects)
{
    for (auto &x: Objects)
    {
        if (!x->GetDrawable())
        {
            continue;
        }
        if (!x->GetEnable())
        {
            continue;
        }
        if (!x->GetComplex())
        {
            fx_Basic *Basic = (fx_Basic*)x;
            Basics[Basic->GetType()].push_back(Basic);
        }
        else
        {
            CombineBasicDFS(Basics, ((fx_Complex*)x)->GetObjects());
        }
    }
    return;
}

void fx_Group::UpdateDFS(std::vector<fx_Objects*> Objects)
{
    for (auto &x: Objects)
    {
        if (!x->GetDrawable())
        {
            continue;
        }
        if (!x->GetEnable())
        {
            continue;
        }
        if (!x->GetComplex())
        {
            if (x->m_FlagUpdateMesh || x->m_FlagUpdateObject)
            {
                x->Update();
                x->m_FlagUpdateMesh = false;
                x->m_FlagUpdateObject = false;
                m_FlagUpdateMesh = true;
            }
        }
        else
        {
            UpdateDFS(((fx_Complex*)x)->GetObjects());
            if (x->m_FlagUpdateMesh || x->m_FlagUpdateObject)
            {
                x->Update();
                x->m_FlagUpdateMesh = false;
                x->m_FlagUpdateObject = false;
                m_FlagUpdateMesh = true;
            }
        }
    }
    return;
}



void fx_Group::GenerateMesh()
{
    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
        if (m_Basics[i].size() == 0)
        {
            continue;
        }
        unsigned int VerticesTotal = 0;
        unsigned int IndicesTotal = 0;
        for (auto x : m_Basics[i])
        {
            fx_Mesh Mesh = x->GetMesh();
            VerticesTotal += Mesh.Vertices.size();
            IndicesTotal += Mesh.Indices.size();
        }

        std::vector<unsigned char> Vertices;
        Vertices.reserve(VerticesTotal);

        std::vector<unsigned int> Indices;
        Indices.reserve(IndicesTotal);

        std::vector<unsigned int> Comp = m_Basics[i][0]->GetMesh().VertexComp;
        std::vector<std::pair<GLenum, GLint>> Type = m_Basics[i][0]->GetMesh().VertexType;

        unsigned int VertexCount = 0;
        for (auto x : m_Basics[i])
        {
            fx_Mesh Mesh = x->GetMesh();
            if (!std::equal(Comp.begin(), Comp.end(), Mesh.VertexComp.begin()))
            {
                std::cout << "Unequal Vertex Component\n";
            }
            if (!std::equal(Type.begin(), Type.end(), Mesh.VertexType.begin()))
            {
                std::cout << "Unequal Vertex Type\n";
            }
            if (Mesh.VertexComp.size() != Mesh.VertexType.size())
            {
                std::cout << "Unequal Vertex Size\n";
            }
            unsigned int VertexSize = 0;

            for (unsigned int j = 0; j < Mesh.VertexComp.size(); j++)
            {
                VertexSize += Mesh.VertexComp[j] * Mesh.VertexType[j].second;
            }
            // unsigned int CompCount = std::reduce(x->VertexComp.begin(), x->VertexComp.end());
            unsigned int IndicesCount = Indices.size();
            Vertices.insert(Vertices.end(), Mesh.Vertices.begin(), Mesh.Vertices.end());
            Indices.insert(Indices.end(), Mesh.Indices.begin(), Mesh.Indices.end());

            std::for_each(Indices.begin() + IndicesCount, Indices.end(), [VertexCount](unsigned int &n){ n+=VertexCount; });
            
            VertexCount += Mesh.Vertices.size() / VertexSize;
        }
        m_Meshes[i] = {Vertices, Indices, Comp, Type};
    }
}

void fx_Group::Update()
{
    for (auto x : m_Objects)
    {
        if (x->m_FlagUpdateObject)
        {
            m_FlagUpdateObject = true;
            break;
        }
    }

    for (auto x : m_Objects)
    {
        if (x->m_FlagUpdateMesh)
        {
            m_FlagUpdateMesh = true;
            break;
        }
    }

    m_FlagUpdateMesh = m_FlagUpdateMesh | m_FlagUpdateObject;

    // for (auto x : m_Objects)
    // {
    //     if (x->GetComplex())
    //     {
    //         fx_Complex *Comp = (fx_Complex *)x;
    //         if (Comp->m_FlagUpdateMesh || Comp->m_FlagUpdateObject)
    //         {
    //             Comp->Update();
    //             Comp->m_FlagUpdateMesh = false;
    //             Comp->m_FlagUpdateObject = false;
    //         }
    //     }
    // }

    UpdateDFS(m_Objects);

    if (m_FlagUpdateObject)
    {
        m_Basics.clear();
        m_Basics.resize(m_Programs.size());
        CombineBasicDFS(m_Basics, m_Objects);
    }

    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
        for (auto x : m_Basics[i])
        {
            if (x->m_FlagUpdateMesh || x->m_FlagUpdateObject)
            {
                x->Update();
                x->m_FlagUpdateMesh = false;
                x->m_FlagUpdateObject = false;
                m_FlagUpdateMesh = true;
            }
        }
    }

    if (m_FlagUpdateMesh)
    {
       GenerateMesh(); 
    }

    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
        m_Buffers[i]->Update(m_Meshes[i]);
    }
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
    m_ProjectionMat = glm::ortho( -m_Aspect * m_Size * 5.0f, m_Aspect * m_Size * 5.0f , -1.0f * m_Size * 5.0f, 1.0f * m_Size * 5.0f, m_Near, m_Far );
    m_Mat = m_ProjectionMat * m_LookAtMat;
    m_InvMat = glm::inverse(m_Mat);
}

void fx_Perspective::UpdateProjectionMat()
{
    m_ProjectionMat = glm::perspective(glm::radians(90.0f), m_Aspect, m_Near, m_Far);
    m_Mat = m_ProjectionMat * m_LookAtMat;
    m_InvMat = glm::inverse(m_Mat);
}

void fx_Group::Draw()
{
    if (m_TextureUnit != NULL)
    {
        m_TextureUnit->Bind();
    }


    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
        if (m_Camera)
        {
            m_Programs[i]->SetUniform(m_Camera->GetMat(), "Matrix");
        }
        if (m_Buffers[i]->GetMeshesIndicesCount() > 0)
        {
            m_Programs[i]->Bind();
            m_Buffers[i]->Bind();
            glDrawElements(GL_TRIANGLES, m_Buffers[i]->GetMeshesIndicesCount(), GL_UNSIGNED_INT, 0);
            m_Programs[i]->Unbind();
            m_Buffers[i]->Unbind();
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
        std::cout << "Framebuffer hasn't complete\n";
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