#include "Group.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

fx_Group::fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit)
{
    m_Programs = Programs;
    m_Basics.resize(m_Programs.size());
    m_Meshes.resize(m_Programs.size());
    m_TextureUnit = TextureUnit;
    m_BHandler = new fx_BillboardHandler();
    m_WHandler = new fx_WidgetHandler();
    m_THandler = new fx_TextHandler();
}

void fx_Group::CreateBuffer()
{
    m_Buffers.resize(m_Programs.size());
    for (unsigned int i = 0; i < m_Buffers.size(); i++)
    {
        m_Buffers[i] = new fx_Buffer(fx_Objects::BasicMeshGenerator((fx_BasicType)i));
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
        else if (!x->GetEnable())
        {
            continue;
        }
        else if (!x->GetComplex())
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
        else if (!x->GetEnable())
        {
            continue;
        }
        else
        {
            if (x->GetNeedUpdate())
            {
                x->Update();
                x->m_FlagUpdateMesh = false;
                x->m_FlagUpdateObject = false;
                m_FlagUpdateMesh = true;
            }
            if (x->GetComplex())
            {
                UpdateDFS(((fx_Complex*)x)->GetObjects());
            }
            continue;
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

        fx_Mesh BasicMesh = fx_Objects::BasicMeshGenerator((fx_BasicType)i);

        unsigned int VertexCount = 0;
        for (auto x : m_Basics[i])
        {
            fx_Mesh Mesh = x->GetMesh();
            if (!std::equal(BasicMesh.VertexComp.begin(), BasicMesh.VertexComp.end(), Mesh.VertexComp.begin()))
            {
                std::cout << "Unequal Vertex Component\n";
            }
            if (!std::equal(BasicMesh.VertexType.begin(), BasicMesh.VertexType.end(), Mesh.VertexType.begin()))
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
        m_Meshes[i] = {Vertices, Indices, BasicMesh.VertexComp, BasicMesh.VertexType};
    }
}

void fx_Group::Update()
{
    if (m_Camera)
    {
        if (m_Camera->GetNeedUpdate())
        {
            m_Camera->Update();
            m_BHandler->SetCameraPos(m_Camera->GetPosition());
            m_BHandler->SetCameraUp(glm::vec3(0,1,0) * m_Camera->GetQuat());
        }
    }

    if (m_Camera && m_FrameBuffer)
    {
        m_THandler->SetPixelDensity((float)(m_FrameBuffer->GetSize().y) / (m_Camera->GetSize() * 2.0f));
    }

    m_BHandler->Update();
    m_WHandler->Update();
    m_THandler->Update();

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

    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;

    UpdateDFS(m_Objects);

    if (!m_FlagUpdateMesh)
    {
        m_FlagUpdateObject = false;
        m_FlagUpdateMesh = false;
        return;
    }

    if (m_FlagUpdateObject)
    {
        m_Basics.clear();
        m_Basics.resize(m_Programs.size());
        CombineBasicDFS(m_Basics, m_Objects);
    }

    GenerateMesh(); 
    // if (m_Buffers.size() == 0)
    // {
    //     CreateBuffer();
    // }
    // for (uint32_t i = 0; i < m_Programs.size(); i++)
    // {
    //     m_Buffers[i]->Update(m_Meshes[i]);
    // }
    m_FlagUpdateBuffer = true;

    m_FlagUpdateObject = false;
    m_FlagUpdateMesh = false;
}


void fx_Group::Draw()
{
    if (m_FlagUpdateBuffer)
    {
        if (m_Buffers.size() == 0)
        {
            CreateBuffer();
        }
        for (uint32_t i = 0; i < m_Programs.size(); i++)
        {
            m_Buffers[i]->Update(m_Meshes[i]);
        }
        m_FlagUpdateBuffer = false;
    }

    if (m_FrameBuffer != NULL)
    {
        m_FrameBuffer->Bind();
    }

    if (m_TextureUnit != NULL)
    {
        m_TextureUnit->Bind();
    }


    for (uint32_t i = 0; i < m_Buffers.size(); i++)
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

    if (m_FrameBuffer != NULL)
    {
        m_FrameBuffer->Unbind();
    }
}