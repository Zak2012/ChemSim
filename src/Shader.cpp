#include "Shader.hpp"

#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <filesystem>
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

#include "ColorConvert.hpp"

// WARNING: DO NOT MULTITHREAD ANY FUNCTION IN THIS FILE
// OPENGL CANNOT BE MULTITHREADED

// #include "File.hpp"

fx_Buffer::fx_Buffer(const fx_Mesh &Triangles)
{
    if (Triangles.VertexComp.size() != Triangles.VertexType.size())
    {
        std::cout << "Mesh Invalid\n";

    }

    unsigned int Stride = 0;

    for (unsigned int i = 0; i < Triangles.VertexComp.size(); i++)
    {
        Stride += Triangles.VertexComp[i] * Triangles.VertexType[i].second;
    }
    
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, Triangles.Vertices.size(), Triangles.Vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.Indices.size() * sizeof(unsigned int), Triangles.Indices.data(), GL_DYNAMIC_DRAW); // flag doesn't matter

    unsigned int Offset = 0;

    for (unsigned int i = 0; i < Triangles.VertexComp.size(); i++)
    {
        glVertexAttribPointer(i, Triangles.VertexComp[i], Triangles.VertexType[i].first, GL_FALSE, Stride, (void *)(Offset));
        glEnableVertexAttribArray(i);
        Offset += Triangles.VertexComp[i] * Triangles.VertexType[i].second;
    }

    m_VerticesCount = Triangles.Vertices.size();
    m_IndicesCount = Triangles.Indices.size();
    Unbind();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void fx_Buffer::Update(const fx_Mesh &Triangles) 
{
    Bind();
    
    if(m_VerticesCount == Triangles.Vertices.size())
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, Triangles.Vertices.size(), Triangles.Vertices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, Triangles.Vertices.size(), Triangles.Vertices.data(), GL_DYNAMIC_DRAW);
        m_VerticesCount = Triangles.Vertices.size();
    }
    if(m_IndicesCount == Triangles.Indices.size())
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Triangles.Indices.size() * sizeof(unsigned int), Triangles.Indices.data());
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Triangles.Indices.size() * sizeof(unsigned int), Triangles.Indices.data(), GL_DYNAMIC_DRAW);
        m_IndicesCount = Triangles.Indices.size();
    }

    unsigned int Stride = 0;

    for (unsigned int i = 0; i < Triangles.VertexComp.size(); i++)
    {
        Stride += Triangles.VertexComp[i] * Triangles.VertexType[i].second;
    }

    unsigned int Offset = 0;

    for (unsigned int i = 0; i < Triangles.VertexComp.size(); i++)
    {
        glVertexAttribPointer(i, Triangles.VertexComp[i], Triangles.VertexType[i].first, GL_FALSE, Stride, (void *)(Offset));
        glEnableVertexAttribArray(i);
        Offset += Triangles.VertexComp[i] * Triangles.VertexType[i].second;
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
    unsigned int GLType = 0;
    if (Type == "vert") {GLType = GL_VERTEX_SHADER;}
    else if (Type == "frag") {GLType = GL_FRAGMENT_SHADER;}
    else {std::cout << "Unknown Type\n";}

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
        std::cout << ErrLog << "\n";
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
        std::cout << ErrLog << "\n";
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
        std::cout << ErrLog << "\n";

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

void fx_Program::SetUniform(float A, std::string Name)
{
    Bind();
    int Location = glGetUniformLocation(GetID(), Name.c_str());
    glUniform1fv(Location, 1, &A);
    Unbind();
}

fx_Texture::fx_Texture(fx_Image &Data, bool Linear)
{
    // OPENGL ES only have RGBA and RGB
    // So we standardise to
    if (Data.Component == 1)
    {
        Data.Data = ColorConvert::Gray2RGBA(Data.Data);
        Data.Component = 4;
    }
    else if (Data.Component == 2)
    {
        Data.Data = ColorConvert::GrayA2RGBA(Data.Data);
        Data.Component = 4;
    }
    else if (Data.Component == 3)
    {
        Data.Data = ColorConvert::RGB2RGBA(Data.Data);
        Data.Component = 4;
    }
    else if (Data.Component == 4)
    {
        
    }
    else
    {
        std::cout << "Unknown Type\n";
    }

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
    glBindRenderbuffer(GL_RENDERBUFFER, m_StencilAttachment);
}

void fx_Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void fx_Framebuffer::ResetBuffer()
{
    Bind();
    glViewport(0, 0, m_Size.x, m_Size.y);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    Unbind();
}
