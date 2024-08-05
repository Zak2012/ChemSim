#pragma once

#include "Object.hpp"

typedef struct fx_Font_Library_s* fx_Font_Library;
typedef struct fx_Face_s* fx_Face;

fx_Font_Library fx_Load_Lib();
void fx_Delete_Lib(fx_Font_Library Lib);
fx_Face fx_Load_Face(fx_Font_Library Lib, std::string FileName);
int fx_Get_Size(fx_Face Face);
void fx_Set_Size(fx_Face Face, int Size);
bool fx_Get_SDF(fx_Face Face);
void fx_Set_SDF(fx_Face Face, bool SDF);
void fx_Delete_Face(fx_Face Face);
fx_Atlas fx_GenerateAtlas(fx_Face Face, bool IsWideChar = true);
void fx_Load_Font(fx_Atlas *InAtlas, fx_Face InFace, std::string Filename, unsigned int Index, bool Wide);

struct fx_CharInfo
{
    float X;
    float Y;
    float Width;
    float Height;
};

// class fx_WText : public fx_Complex
// {
// protected:
//     std::vector<fx_CharInfo> GetTextLayout(std::wstring Text);
// public:
//     glm::vec2 m_GlowThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
//     glm::vec4 m_GlowColor = {1,1,1,1}; // non sdf face will ignore these
//     glm::vec2 m_OutlineThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
//     glm::vec4 m_OutlineColor = {1,1,1,1}; // non sdf face will ignore these
//     fx_Face m_Face;
//     fx_Atlas m_FontAtlas;
//     unsigned int m_FontOffset;
//     std::wstring m_Text;
//     float m_Kerning = 0.0f;
//     fx_WText(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::wstring Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color = {1,1,1,1});
//     void Update();

// };

class fx_Text : public fx_Complex
{
protected:
    std::vector<fx_CharInfo> GetTextLayout(std::string Text);
public:
    glm::vec2 m_GlowThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
    glm::vec4 m_GlowColor = {1,1,1,1}; // non sdf face will ignore these
    glm::vec2 m_OutlineThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
    glm::vec4 m_OutlineColor = {1,1,1,1}; // non sdf face will ignore these
    fx_Face m_Face;
    fx_Atlas m_FontAtlas;
    unsigned int m_FontOffset;
    std::string m_Text;
    float m_Kerning = 0.0f;
    fx_Text(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::string Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color = {1,1,1,1});
    void Update();
};