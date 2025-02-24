#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include <glm/glm.hpp>

#include "Resource.hpp"

struct fx_Mesh
{
    std::vector<unsigned char> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<unsigned int> VertexComp;
    std::vector<std::pair<unsigned int, int>> VertexType;//type+size
};

class fx_Buffer
{
private:
    fx_Mesh m_Mesh;
    unsigned int m_VAO, m_VBO, m_EBO;
public:
    fx_Buffer(fx_Mesh Triangles);
    fx_Buffer(fx_Mesh Triangles, unsigned int VAO, unsigned int VBO, unsigned int EBO);
    ~fx_Buffer();
    void Bind();
    void Unbind();
    void Update(fx_Mesh Triangles);
    fx_Mesh GetMesh() {return m_Mesh;}
    unsigned int GetMeshesVerticesCount() {return m_Mesh.Vertices.size();}
    unsigned int GetMeshesIndicesCount() {return m_Mesh.Indices.size();}
    unsigned int GetVAO() {return m_VAO;}
    unsigned int GetVBO() {return m_VBO;}
    unsigned int GetEBO() {return m_EBO;}
};

class fx_Shader
{
private:
    unsigned int m_ShaderID;
public:
    fx_Shader(unsigned int ShaderID) {m_ShaderID = ShaderID;}
    fx_Shader(std::string Source, std::string Type);
    ~fx_Shader();
    unsigned int GetID() {return m_ShaderID;}
};

class fx_Program
{
private:
    unsigned int m_ProgramID;
public:
    fx_Program(unsigned int ProgramID) {m_ProgramID = ProgramID;}
    fx_Program(std::vector<unsigned int> ShaderIDs);
    fx_Program(std::vector<fx_Shader*> Shaders);
    ~fx_Program();
    void SetUniform(glm::mat4 A, std::string Name);
    void SetUniform(float A, std::string Name);
    void Bind();
    void Unbind();
    unsigned int GetID() {return m_ProgramID;}
};

class fx_Texture
{
private:
    unsigned int m_TextureID;
    fx_Image m_Data;
public:
    fx_Texture(fx_Image &Data, bool Linear=true);
    ~fx_Texture();
    void Update(fx_Image &Data);
    fx_Image GetData() {return m_Data;}
    unsigned int GetID() {return m_TextureID;}
    void Bind();
    void Unbind();
};