#pragma once

#include "Object.hpp"

class fx_Font
{
protected:
    void *m_FontFace = NULL;
    unsigned int m_FontId = 0;
    std::vector<uint8_t> m_FontCache;

    void InitRuntime();
    fx_Image RenderChar(uint32_t GlyphIndex, unsigned int RenderMode);
    // void CreateAtlas();
public:
    fx_Atlas *m_Atlas = nullptr;
    fx_Font(std::string FontPath);
    fx_Font(std::vector<uint8_t> Buffer);
    ~fx_Font();

    void *GetInternalFontFace(){return m_FontFace;}
    std::string GetFontID(){return "Font_" + std::to_string(m_FontId);}
    void RenderFont();
friend class fx_Text;
};

class fx_TextBase : public fx_Complex
{
protected:
    fx_Font *m_Font = NULL;
    std::string m_Text;
    float m_Kerning = 0.0f;
    float m_LineHeight = 0.0f;
    float m_PixelDensity = -1.0f;

public:
    virtual fx_Font *GetFont(){return m_Font;}
    virtual std::string GetText(){return m_Text;}
    virtual float GetLineHeight(){return m_LineHeight;}
    virtual float GetKerning(){return m_Kerning;}
    virtual float GetPixelDensity(){return m_PixelDensity;}

    virtual void SetFont(fx_Font *Font){m_FlagUpdateMesh |= m_Font!=Font; m_Font = Font;}
    virtual void SetText(std::string Text){m_FlagUpdateObject |= m_Text!=Text; m_Text = Text;}
    virtual void SetLineHeight(float LineHeight){m_FlagUpdateMesh |= m_LineHeight!=LineHeight; m_LineHeight = LineHeight;}
    virtual void SetKerning(float Kerning){m_FlagUpdateMesh |= m_Kerning!=Kerning; m_Kerning = Kerning;}
    virtual void SetPixelDensity(float PixelDensity){m_FlagUpdateMesh |= m_PixelDensity!=PixelDensity; m_PixelDensity = PixelDensity;}
    virtual void Update(){};
};

class fx_Text : public fx_TextBase
{
protected:
    std::vector<fx_SDF*> m_TextObj;
    static float PixelSnap(float a, float Pixeldensity){return std::round(a * Pixeldensity) / Pixeldensity;};
    static glm::vec2 PixelSnap(glm::vec2 a, float Pixeldensity){return glm::round(a * Pixeldensity) / Pixeldensity;};
    static glm::vec3 PixelSnap(glm::vec3 a, float Pixeldensity){return glm::round(a * Pixeldensity) / Pixeldensity;};
public:
    fx_Text(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour = {1,1,1,1}, glm::vec4 Background = {0,0,0,1});
    virtual ~fx_Text() {for(auto x : m_Objects){delete (fx_SDF*)x;}}
    
    void SetText(std::string Text);
    void Update();
    static std::vector<glm::vec4> GetTextLayout(std::string Text, float LineHeight, float Kerning, fx_Font *Font);

};

class fx_TextBox : public fx_TextBase
{
protected:
    float m_LineSpacing = 1.0f;
    std::vector<fx_Text*> m_Lines;
    std::vector<std::string> m_LineText;
    fx_Quad* m_Bg;
    // -1:left, 0:centre, 1:right
    float m_Align = 0.0f;
public:

    fx_TextBox(glm::vec3 Pos, float LineHeight, float Width, fx_Font *Font, std::string Text, glm::vec4 Colour = {1,1,1,1}, glm::vec4 Background = {0,0,0,1});
    virtual ~fx_TextBox() {for(auto x : m_Lines){delete x;} delete m_Bg;}

    void SetText(std::string Text);
    void Update();
    
    float GetLineSpacing(){return m_LineSpacing;}
    float GetAlign(){return m_Align;}
    glm::vec4 GetBackgroundColour(){return m_Bg->GetColour();}

    void SetLineSpacing(float LineSpacing){m_FlagUpdateMesh |= m_LineSpacing!=LineSpacing; m_LineSpacing = LineSpacing;}
    void SetAlign(float Align){m_FlagUpdateMesh |= m_Align!=Align; m_Align = Align;}
    void SetBackgroundColour(glm::vec4 Bg){m_Bg->SetColour(Bg);}

    static std::vector<std::string> Tokenize(std::string Text);
    static std::vector<std::string> Box(float Width, float LineHeight, float Kerning, fx_Font *Font,std::string Text);
};

class fx_TextHandler
{
protected:
    std::set<fx_TextBase*> m_TextObj;
    float m_PixelDensity = -1.0f;
public:
    float GetPixelDensity(){return m_PixelDensity;}
    void SetPixelDensity(float PixelDensity){m_PixelDensity = PixelDensity;}

    void AddObject(fx_TextBase *Obj){m_TextObj.insert(Obj);}
    void DelObject(fx_TextBase *Obj){m_TextObj.erase(Obj);}

    virtual void Update(){for(auto x:m_TextObj){x->SetPixelDensity(m_PixelDensity);}}

};

