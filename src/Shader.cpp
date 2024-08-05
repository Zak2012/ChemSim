#define GLEW_STATIC
#include "Shader.hpp"

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "File.hpp"

fx_Buffer::fx_Buffer(fx_Mesh Triangles)
{
    if (m_Mesh.VertexComp.size() != m_Mesh.VertexType.size())
    {
        throw std::runtime_error("Mesh Invalid");
    }

    unsigned int Stride = 0;

    for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)
    {
        Stride += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;
    }

    m_Mesh = Triangles;
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Mesh.Vertices.size(), m_Mesh.Vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Mesh.Indices.size() * sizeof(unsigned int), m_Mesh.Indices.data(), GL_DYNAMIC_DRAW); // flag doesn't matter

    unsigned int Offset = 0;

    for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)
    {
        glVertexAttribPointer(i, m_Mesh.VertexComp[i], m_Mesh.VertexType[i].first, GL_FALSE, Stride, (void *)(Offset));
        glEnableVertexAttribArray(i);
        Offset += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;
    }

    Unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

fx_Buffer::fx_Buffer(fx_Mesh Triangles, unsigned int VAO, unsigned int VBO, unsigned int EBO)
{
    m_VAO = VAO;
    m_VBO = VBO;
    m_EBO = EBO;
    m_Mesh = Triangles;
}

void fx_Buffer::Update(fx_Mesh Triangles) 
{
    Bind();
    
    if(m_Mesh.Vertices.size() == Triangles.Vertices.size())
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, Triangles.Vertices.size(), Triangles.Vertices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, Triangles.Vertices.size(), Triangles.Vertices.data(), GL_DYNAMIC_DRAW);
    }
    if(m_Mesh.Indices.size() == Triangles.Indices.size())
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Triangles.Indices.size() * sizeof(unsigned int), Triangles.Indices.data());
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.Indices.size() * sizeof(unsigned int), Triangles.Indices.data(), GL_DYNAMIC_DRAW);
    }
    m_Mesh = Triangles;

    unsigned int Stride = 0;

    for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)
    {
        Stride += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;
    }

    unsigned int Offset = 0;

    for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)
    {
        glVertexAttribPointer(i, m_Mesh.VertexComp[i], m_Mesh.VertexType[i].first, GL_FALSE, Stride, (void *)(Offset));
        glEnableVertexAttribArray(i);
        Offset += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;
    }
    Unbind();
}

void fx_Buffer::Bind()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}

void fx_Buffer::Unbind()
{
    glBindVertexArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

fx_Buffer::~fx_Buffer()
{
    Unbind();
    glDeleteVertexArrays(1 , &m_VAO);
    glDeleteBuffers(1 , &m_VBO);
    glDeleteBuffers(1 , &m_EBO);
}

fx_Shader::fx_Shader(std::string Source, std::string Type)
{
    int SuccessCode;
    std::string ErrLog;
    unsigned int GLType;
    if (Type == "vert") {GLType = GL_VERTEX_SHADER;}
    else if (Type == "frag") {GLType = GL_FRAGMENT_SHADER;}
    else {throw std::runtime_error("Unknown Type");}

    const char *SourceCstr = Source.c_str();
    m_ShaderID = glCreateShader(GLType);
    glShaderSource(m_ShaderID, 1, &SourceCstr, NULL);
    glCompileShader(m_ShaderID);

    glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &SuccessCode);
    if ( !SuccessCode )
    {
        int ErrLenght;
        glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &ErrLenght);
        ErrLog.resize(ErrLenght);
        glGetShaderInfoLog(m_ShaderID, ErrLenght, NULL, ErrLog.data());
        glDeleteShader(m_ShaderID);
        throw std::runtime_error(ErrLog);
    }
}

fx_Shader::~fx_Shader()
{
    glDeleteShader(m_ShaderID);
}

fx_Program::fx_Program(std::vector<unsigned int> ShaderIDs)
{
    int SuccessCode;
    std::string ErrLog;

    m_ProgramID = glCreateProgram();
    for (auto x: ShaderIDs)
    {
        glAttachShader(m_ProgramID, x);
    }
    glLinkProgram(m_ProgramID);

    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &SuccessCode);
    if ( !SuccessCode )
    {
        int ErrLenght;
        glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &ErrLenght);
        glGetProgramInfoLog(m_ProgramID, ErrLenght, NULL, ErrLog.data());
        throw std::runtime_error(ErrLog);
    }
}

fx_Program::fx_Program(std::vector<fx_Shader*> Shaders)
{
    int SuccessCode;
    std::string ErrLog;

    m_ProgramID = glCreateProgram();
    for (auto x: Shaders)
    {
        glAttachShader(m_ProgramID, x->GetID());
    }
    glLinkProgram(m_ProgramID);

    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &SuccessCode);
    if ( !SuccessCode )
    {
        int ErrLenght;
        glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &ErrLenght);
        glGetProgramInfoLog(m_ProgramID, ErrLenght, NULL, ErrLog.data());
        throw std::runtime_error(ErrLog);
    }
}

fx_Program::~fx_Program()
{
    glDeleteProgram(m_ProgramID);
}

void fx_Program::Bind()
{
    glUseProgram(m_ProgramID);
}

void fx_Program::Unbind()
{
    glUseProgram(0);
}

void fx_Program::SetUniform(glm::mat4 A, std::string Name)
{
    Bind();
    int Location = glGetUniformLocation(GetID(), Name.c_str());
    glUniformMatrix4fv(Location, 1, GL_FALSE, &A[0][0]);
    Unbind();
}


void fx_Batch::GenerateMesh()
{
    if (m_Data.size() == 0)
    {
        return;
    }
    unsigned int VerticesTotal = 0;
    unsigned int IndicesTotal = 0;
    for (auto x : m_Data)
    {
        VerticesTotal += x->Vertices.size();
        IndicesTotal += x->Indices.size();
    }

    std::vector<unsigned char> Vertices;
    Vertices.reserve(VerticesTotal);

    std::vector<unsigned int> Indices;
    Indices.reserve(IndicesTotal);

    std::vector<unsigned int> Comp = m_Data[0]->VertexComp;
    std::vector<std::pair<GLenum, GLint>> Type = m_Data[0]->VertexType;

    unsigned int VertexCount = 0;
    for (auto x : m_Data)
    {
        if (!std::equal(Comp.begin(), Comp.end(), x->VertexComp.begin()))
        {
            throw std::runtime_error("Unequal Vertex Component");
        }
        if (!std::equal(Type.begin(), Type.end(), x->VertexType.begin()))
        {
            throw std::runtime_error("Unequal Vertex Type");
        }
        if (x->VertexComp.size() != x->VertexType.size())
        {
            throw std::runtime_error("Mesh Invalid");
        }
        unsigned int VertexSize = 0;

        for (unsigned int i = 0; i < x->VertexComp.size(); i++)
        {
            VertexSize += x->VertexComp[i] * x->VertexType[i].second;
        }
        // unsigned int CompCount = std::reduce(x->VertexComp.begin(), x->VertexComp.end());
        unsigned int IndicesCount = Indices.size();
        Vertices.insert(Vertices.end(), x->Vertices.begin(), x->Vertices.end());
        Indices.insert(Indices.end(), x->Indices.begin(), x->Indices.end());

        std::for_each(Indices.begin() + IndicesCount, Indices.end(), [VertexCount](unsigned int &n){ n+=VertexCount; });
        
        VertexCount += x->Vertices.size() / VertexSize;
    }
    m_Mesh = {Vertices, Indices, Comp};
}

fx_Texture::fx_Texture(fx_Image &Data, bool Linear)
{
    m_Data = Data;
    GLenum Format[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

    glGenTextures(1,&m_TextureID);
    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, Format[Data.Component-1], m_Data.Width, m_Data.Height, 0, Format[Data.Component-1], GL_UNSIGNED_BYTE, 0);
    if (m_Data.Data.size()>0)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Data.Width, m_Data.Height, Format[Data.Component-1], GL_UNSIGNED_BYTE, m_Data.Data.data());
    }
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, Linear? GL_LINEAR: GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, Linear? GL_LINEAR: GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    m_Data.Data.clear();
    Unbind();
}

fx_Texture::~fx_Texture()
{
    glDeleteTextures(1, &m_TextureID);
}

void fx_Texture::Update(fx_Image &Data)
{
    Bind();
    GLenum Format[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

    if (m_Data.Width != Data.Width || m_Data.Height != Data.Height || m_Data.Component != Data.Component)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, Format[Data.Component-1], Data.Width, Data.Height, 0, Format[Data.Component-1], GL_UNSIGNED_BYTE, 0);
    }

    m_Data = Data;
    if (m_Data.Data.size() > 0)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Data.Width, m_Data.Height, Format[m_Data.Component-1], GL_UNSIGNED_BYTE, m_Data.Data.data());
    }
    m_Data.Data.clear();
    Unbind();
}

void fx_Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}
void fx_Texture::Unbind() 
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

