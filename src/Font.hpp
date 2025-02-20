#pragma once

#include "Object.hpp"

// typedef struct fx_Font_Library_s* fx_Font_Library;
// typedef struct fx_Face_s* fx_Face;

// fx_Font_Library fx_Load_Lib();
// void fx_Delete_Lib(fx_Font_Library Lib);
// fx_Face fx_Load_Face(fx_Font_Library Lib, std::string FileName);
// int fx_Get_Size(fx_Face Face);
// void fx_Set_Size(fx_Face Face, int Size);
// bool fx_Get_SDF(fx_Face Face);
// void fx_Set_SDF(fx_Face Face, bool SDF);
// void fx_Delete_Face(fx_Face Face);
// fx_Atlas fx_GenerateAtlas(fx_Face Face, bool IsWideChar = true);
// void fx_Load_Font(fx_Atlas *InAtlas, fx_Face InFace, std::string Filename, unsigned int Index, bool Wide);

class fx_Font
{
protected:
    void *m_FontFace = NULL;
    fx_Atlas m_CharAtlas;
    std::vector<uint8_t> m_FontCache;

    void InitRuntime();
    fx_Image RenderChar(uint32_t Code);
    void CreateAtlas();
public:
    fx_Font(std::string FontPath);
    fx_Font(std::vector<uint8_t> Buffer);
    ~fx_Font();

    fx_Atlas GetAtlas(){return m_CharAtlas;}
friend class fx_TextBox;
};

// struct fx_CharInfo
// {
//     float X;
//     float Y;
//     float Width;
//     float Height;
// };

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
class fx_TextBox : public fx_Complex
{
protected:
    glm::vec2 m_GlowThreshold = {0.5f, 0.5f};
    glm::vec4 m_GlowColour = {1,1,1,1};
    glm::vec2 m_OutlineThreshold = {0.5f, 0.5f};
    glm::vec4 m_OutlineColour = {1,1,1,1};
    fx_Font *m_Font = NULL;
    std::string m_Text;
    float m_Kerning = 0.0f;
    float m_LineHeight = 0.0f;
    std::vector<glm::vec4> GetTextLayout(std::string Text);
public:
    fx_TextBox(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour = {1,1,1,1}, glm::vec4 Background = {0,0,0,1});
    ~fx_TextBox() {for(auto x : m_Objects){delete x;}}

    void Update();

    fx_Font *GetFont(){return m_Font;}
    std::string GetText(){return m_Text;}
    float GetLineHeight(){return m_LineHeight;}
    float GetKerning(){return m_Kerning;}
    glm::vec2 GetGlowThreshold(){return m_GlowThreshold;}
    glm::vec4 GetGlowColour(){return m_GlowColour;}
    glm::vec2 GetOutlineThreshold(){return m_OutlineThreshold;}
    glm::vec4 GetOutlineColour(){return m_OutlineColour;}

    void SetFont(fx_Font *Font){m_FlagUpdateMesh = m_Font!=Font; m_Font = Font;}
    void SetText(std::string Text){m_FlagUpdateObject = m_Text!=Text; m_Text = Text;}
    void SetLineHeight(float LineHeight){m_FlagUpdateMesh = m_LineHeight!=LineHeight; m_LineHeight = LineHeight;}
    void SetKerning(float Kerning){m_FlagUpdateMesh = m_Kerning!=Kerning; m_Kerning = Kerning;}
    void SetGlowTreshold(glm::vec2 GlowThreshold){m_FlagUpdateMesh = m_GlowThreshold!=GlowThreshold; m_GlowThreshold = GlowThreshold;}
    void SetGlowColour(glm::vec4 GlowColour){m_FlagUpdateMesh = m_GlowColour!=GlowColour; m_GlowColour = GlowColour;}
    void SetOutlineColour(glm::vec4 OutlineColour){m_FlagUpdateMesh = m_OutlineColour!=OutlineColour; m_OutlineColour = OutlineColour;}
    void SetOutlineTreshold(glm::vec2 OutlineTreshold){m_FlagUpdateMesh = m_OutlineThreshold!=OutlineTreshold; m_OutlineThreshold = OutlineTreshold;}

};

// class fx_Text : public fx_Complex
// {
// protected:
//     std::vector<fx_CharInfo> GetTextLayout(std::string Text);
// public:
//     glm::vec2 m_GlowThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
//     glm::vec4 m_GlowColor = {1,1,1,1}; // non sdf face will ignore these
//     glm::vec2 m_OutlineThreshold = {0.5f, 0.5f}; // non sdf face will ignore these
//     glm::vec4 m_OutlineColor = {1,1,1,1}; // non sdf face will ignore these
//     fx_Face m_Face;
//     fx_Atlas m_FontAtlas;
//     unsigned int m_FontOffset;
//     std::string m_Text;
//     float m_Kerning = 0.0f;
//     fx_Text(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::string Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color = {1,1,1,1});
//     void Update();
// };

