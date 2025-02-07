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

void fx_Basic::Update()
{
    if (!m_FlagUpdateMesh)
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

fx_Triangle::fx_Triangle(glm::vec3 Pos, glm::vec2 Size, std::vector<glm::vec3> Vertices, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Vertices = Vertices;
    m_Complex = false;
    m_Drawable = true;
    m_Type = fx_BasicType::Basic;
}

void fx_Triangle::GenerateMesh()
{
    union fx_m_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
        } v;
        uint8_t raw[sizeof(v)];
    };
    m_Mesh = {{},{0,1,2},{sizeof(glm::vec3),sizeof(glm::vec4)}, {}};
    for (uint32_t i = 0; i < m_Mesh.VertexComp.size(); i++) { m_Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    m_Mesh.Vertices.reserve(sizeof(fx_m_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_m_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_m_Mesh::raw)]);
    }
}

fx_Quad::fx_Quad(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Complex = false;
    m_Drawable = true;
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
    union fx_m_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
        } v;
        uint8_t raw[sizeof(v)];
    };
    m_Mesh = {{},{0,1,2, 0,2,3},{sizeof(glm::vec3),sizeof(glm::vec4)}, {}};
    for (uint32_t i = 0; i < m_Mesh.VertexComp.size(); i++) { m_Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    m_Mesh.Vertices.reserve(sizeof(fx_m_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_m_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_m_Mesh::raw)]);
    }
}

fx_Sprite::fx_Sprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    SetUV(UV);
    m_Complex = false;
    m_Drawable = true;
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
    union fx_m_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
            glm::vec2 uv;
        }v;
        uint8_t raw[sizeof(v)];
    };
    m_Mesh = {{},{0,1,2, 0,2,3},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2)}, {}};
    for (uint32_t i = 0; i < m_Mesh.VertexComp.size(); i++) { m_Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    m_Mesh.Vertices.reserve(sizeof(fx_m_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_m_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;

        if ( i == 0 ){temp.v.uv =      {m_UV.X1, m_UV.Y2};}
        else if ( i == 1 ){temp.v.uv = {m_UV.X2, m_UV.Y2};}
        else if ( i == 2 ){temp.v.uv = {m_UV.X2, m_UV.Y1};}
        else if ( i == 3 ){temp.v.uv = {m_UV.X1, m_UV.Y1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_m_Mesh::raw)]);
    }
}

fx_Circle::fx_Circle(glm::vec3 Pos, glm::vec2 Size, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Drawable = true;
    m_Complex = false;
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
    union fx_m_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
            glm::vec2 uv;
            float Outline;
            float Angle;
        }v;
        uint8_t raw[sizeof(v)];
    };
    m_Mesh = {{},{0,1,2, 0,2,3},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2), sizeof(float), sizeof(float)},{}};
    for (uint32_t i = 0; i < m_Mesh.VertexComp.size(); i++) { m_Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    m_Mesh.Vertices.reserve(sizeof(fx_m_Mesh::raw) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_m_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.Outline = m_Outline;
        temp.v.Angle = m_Angle;

        if ( i == 0 ){temp.v.uv =      {-1, -1};}
        else if ( i == 1 ){temp.v.uv = { 1, -1};}
        else if ( i == 2 ){temp.v.uv = { 1,  1};}
        else if ( i == 3 ){temp.v.uv = {-1,  1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_m_Mesh::raw)]);
    }
}

fx_SDF::fx_SDF(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Colour)
{
    SetPosition(Pos);
    SetCube(glm::vec3(Size,1));
    SetColour(Colour);
    m_Drawable = true;
    m_Complex = false;
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
    union fx_m_Mesh
    {
        struct
        {
            glm::vec3 pos;
            glm::vec4 col;
            glm::vec2 uv;
            glm::vec2 Gt;
            glm::vec4 Gcol;
            glm::vec2 Ot;
            glm::vec4 OCol;
        }v;
        uint8_t raw[sizeof(v)];
    };
    m_Mesh = {{},{0,1,2, 0,2,3},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2),sizeof(glm::vec2), sizeof(glm::vec4), sizeof(glm::vec2), sizeof(glm::vec4)},{}};
    for (uint32_t i = 0; i < m_Mesh.VertexComp.size(); i++) { m_Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
    m_Mesh.Vertices.reserve(sizeof(fx_m_Mesh) * m_ModelVertices.size());
    for (unsigned int i = 0; i < m_ModelVertices.size(); i++)
    {
        fx_m_Mesh temp;
        temp.v.pos = m_ModelVertices[i];
        temp.v.col = m_Colour;
        temp.v.Gt = m_GlowThreshold;
        temp.v.Gcol = m_GlowColour;
        temp.v.Ot = m_OutlineThreshold;
        temp.v.OCol = m_OutlineColour;

        if ( i == 0 ){temp.v.uv =      {m_UV.X1, m_UV.Y2};}
        else if ( i == 1 ){temp.v.uv = {m_UV.X2, m_UV.Y2};}
        else if ( i == 2 ){temp.v.uv = {m_UV.X2, m_UV.Y1};}
        else if ( i == 3 ){temp.v.uv = {m_UV.X1, m_UV.Y1};}
        m_Mesh.Vertices.insert(m_Mesh.Vertices.end(), &temp.raw[0], &temp.raw[sizeof(fx_m_Mesh::raw)]);
    }
}

fx_Group::fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit)
{
    m_Programs = Programs;
    m_Buffers.resize(m_Programs.size());
    m_Basics.resize(m_Programs.size());
    m_Meshes.resize(m_Programs.size());
    m_TextureUnit = TextureUnit;
    {
        fx_Mesh Mesh = {{},{},{sizeof(glm::vec3),sizeof(glm::vec4)}, {}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Basic] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2)}, {}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Sprite] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2), sizeof(float), sizeof(float)},{}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::Circle] = new fx_Buffer(Mesh);
    }
    {
        fx_Mesh Mesh = {{},{0,1,2, 0,2,3},{sizeof(glm::vec3),sizeof(glm::vec4), sizeof(glm::vec2),sizeof(glm::vec2), sizeof(glm::vec4), sizeof(glm::vec2), sizeof(glm::vec4)},{}};
        for (uint32_t i = 0; i < Mesh.VertexComp.size(); i++) { Mesh.VertexType.push_back({GL_FLOAT, sizeof(float)});}
        m_Buffers[fx_BasicType::SDF] = new fx_Buffer(Mesh);
    }
}

void dfs(std::vector<std::vector<fx_Basic*>> Basics, std::vector<fx_Objects*> Objects)
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
            
            // Basic->SetBatch(Bacthes[Basic->GetType()]);
            // std::cout << Bacthes[Basic->GetType()]->GetMesh().Vertices.size() << "\n";
        }
        else
        {
            dfs(Basics, ((fx_Complex*)x)->GetObjects());
        }
    }
    return;
}


void fx_Group::GenerateMesh()
{
    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
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
                throw std::runtime_error("Unequal Vertex Component");
            }
            if (!std::equal(Type.begin(), Type.end(), Mesh.VertexType.begin()))
            {
                throw std::runtime_error("Unequal Vertex Type");
            }
            if (Mesh.VertexComp.size() != Mesh.VertexType.size())
            {
                throw std::runtime_error("Mesh Invalid");
            }
            unsigned int VertexSize = 0;

            for (unsigned int i = 0; i < Mesh.VertexComp.size(); i++)
            {
                VertexSize += Mesh.VertexComp[i] * Mesh.VertexType[i].second;
            }
            // unsigned int CompCount = std::reduce(x->VertexComp.begin(), x->VertexComp.end());
            unsigned int IndicesCount = Indices.size();
            Vertices.insert(Vertices.end(), Mesh.Vertices.begin(), Mesh.Vertices.end());
            Indices.insert(Indices.end(), Mesh.Indices.begin(), Mesh.Indices.end());

            std::for_each(Indices.begin() + IndicesCount, Indices.end(), [VertexCount](unsigned int &n){ n+=VertexCount; });
            
            VertexCount += Mesh.Vertices.size() / VertexSize;
        }
        m_Meshes[i] = {Vertices, Indices, Comp};
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
        if (x->GetComplex())
        {
            fx_Complex *Comp = (fx_Complex *)x;
            if (Comp->m_FlagUpdateMesh || Comp->m_FlagUpdateObject)
            {
                Comp->Update();
                Comp->m_FlagUpdateMesh = false;
                Comp->m_FlagUpdateObject = false;
            }
        }
    }

    if (m_FlagUpdateObject)
    {
        m_Basics.clear();
        m_Basics.resize(m_Programs.size());
        dfs(m_Basics, m_Objects);
    }

    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
        for (auto x : m_Basics[i])
        {
            if (x->m_FlagUpdateMesh)
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

void fx_Group::Draw()
{
    if (m_TextureUnit != NULL)
    {
        m_TextureUnit->Bind();
    }

    for (uint32_t i = 0; i < m_Programs.size(); i++)
    {
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

