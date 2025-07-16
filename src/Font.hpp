#pragma once

#include "Object.hpp"

class fx_Font
{
protected:
    void *m_FontFace = NULL;
    // void *m_FontBlob = NULL;
    // void *m_FontFace2 = NULL;
    // void *m_Font = NULL;
    fx_Atlas m_CharAtlas;
    std::vector<uint8_t> m_FontCache;

    void InitRuntime();
    fx_Image RenderChar(uint32_t Char, uint32_t GlyphIndex);
    void CreateAtlas();
public:
    fx_Font(std::string FontPath);
    fx_Font(std::vector<uint8_t> Buffer);
    ~fx_Font();

    void *GetInternalFontFace(){return m_FontFace;}
    fx_Atlas GetAtlas(){return m_CharAtlas;}
friend class fx_Text;
};

class fx_Text : public fx_Complex
{
protected:
    // glm::vec2 m_GlowThreshold = {0.5f, 0.5f};
    // glm::vec4 m_GlowColour = {1,1,1,1};
    // glm::vec2 m_OutlineThreshold = {0.5f, 0.5f};
    // glm::vec4 m_OutlineColour = {1,1,1,1};
    fx_Font *m_Font = NULL;
    std::string m_Text;
    float m_Kerning = 0.0f;
    float m_LineHeight = 0.0f;
    float m_Ascender = 0.0f;
    float m_Descender = 0.0f;
    public:
    fx_Text(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour = {1,1,1,1}, glm::vec4 Background = {0,0,0,1});
    virtual ~fx_Text() {for(auto x : m_Objects){delete (fx_SDF*)x;}}
    
    void Update();
    
    fx_Font *GetFont(){return m_Font;}
    std::string GetText(){return m_Text;}
    float GetLineHeight(){return m_LineHeight;}
    float GetAscender(){return m_Ascender;}
    float GetDescender(){return m_Descender;}
    float GetKerning(){return m_Kerning;}
    static std::vector<glm::vec4> GetTextLayout(std::string Text, float LineHeight, float Kerning, fx_Font *Font);
    // glm::vec2 GetGlowThreshold(){return m_GlowThreshold;}
    // glm::vec4 GetGlowColour(){return m_GlowColour;}
    // glm::vec2 GetOutlineThreshold(){return m_OutlineThreshold;}
    // glm::vec4 GetOutlineColour(){return m_OutlineColour;}

        void SetFont(fx_Font *Font){m_FlagUpdateMesh = m_Font!=Font; m_Font = Font;}
        void SetText(std::string Text){m_FlagUpdateObject = m_Text!=Text; m_Text = Text;}
        void SetLineHeight(float LineHeight){m_FlagUpdateMesh = m_LineHeight!=LineHeight; m_LineHeight = LineHeight;}
        void SetKerning(float Kerning){m_FlagUpdateMesh = m_Kerning!=Kerning; m_Kerning = Kerning;}
    // void SetGlowTreshold(glm::vec2 GlowThreshold){m_FlagUpdateMesh = m_GlowThreshold!=GlowThreshold; m_GlowThreshold = GlowThreshold;}
    // void SetGlowColour(glm::vec4 GlowColour){m_FlagUpdateMesh = m_GlowColour!=GlowColour; m_GlowColour = GlowColour;}
    // void SetOutlineColour(glm::vec4 OutlineColour){m_FlagUpdateMesh = m_OutlineColour!=OutlineColour; m_OutlineColour = OutlineColour;}
    // void SetOutlineTreshold(glm::vec2 OutlineTreshold){m_FlagUpdateMesh = m_OutlineThreshold!=OutlineTreshold; m_OutlineThreshold = OutlineTreshold;}

};

class fx_TextBox : public fx_Complex
{
protected:
    fx_Font *m_Font = NULL;
    std::string m_Text;
    float m_Kerning = 0.0f;
    float m_LineHeight = 0.0f;
    float m_LineSpacing = 1.0f;
    std::vector<fx_Text*> m_Lines;
    fx_Quad* m_Bg;
    // -1:left, 0:centre, 1:right
    float m_Align = 0.0f;
public:

    fx_TextBox(glm::vec3 Pos, float LineHeight, float Width, fx_Font *Font, std::string Text, glm::vec4 Colour = {1,1,1,1}, glm::vec4 Background = {0,0,0,1});
    virtual ~fx_TextBox() {for(auto x : m_Lines){delete x;} delete m_Bg;}

    void Update();
    
    fx_Font *GetFont(){return m_Font;}
    std::string GetText(){return m_Text;}
    float GetLineSpacing(){return m_LineSpacing;}
    float GetLineHeight(){return m_LineHeight;}
    float GetKerning(){return m_Kerning;}
    float GetAlign(){return m_Align;}
    glm::vec4 GetBackgroundColour(){return m_Bg->GetColour();}

    void SetFont(fx_Font *Font){m_FlagUpdateMesh = m_Font!=Font; m_Font = Font;}
    void SetText(std::string Text){m_FlagUpdateObject = m_Text!=Text; m_Text = Text;}
    void SetLineSpacing(float LineSpacing){m_FlagUpdateMesh = m_LineSpacing!=LineSpacing; m_LineSpacing = LineSpacing;}
    void SetLineHeight(float LineHeight){m_FlagUpdateMesh = m_LineHeight!=LineHeight; m_LineHeight = LineHeight;}
    void SetKerning(float Kerning){m_FlagUpdateMesh = m_Kerning!=Kerning; m_Kerning = Kerning;}
    void SetAlign(float Align){m_FlagUpdateMesh = m_Align!=Align; m_Align = Align;}
    void SetBackgroundColour(glm::vec4 Bg){m_Bg->SetColour(Bg);}

    static std::vector<std::string> Tokenize(std::string Text);
    static std::vector<std::string> Box(float Width, float LineHeight, float Kerning, fx_Font *Font,std::string Text);
};

