#include "Font.hpp"

#include <string>
#include <vector>
#include <exception>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

// #include <msdfgen/msdfgen.h>
// #include <msdfgen/msdfgen-ext.h>

#include "Resource.hpp"

#define FONT_SIZE_PIXEL 256
#define FONT_SIZE_PIXEL_SMALL 32

const static std::vector<unsigned int> BitmapPixels = {64, 56, 48, 40, 32, 24, 20, 16, 14, 12, 10, 6};

static FT_Library FTRuntime = NULL;
static uint32_t FontCount = 0;


std::string Escape(std::string s)
{
  std::string out = "";
  out += '"';
  for (std::string::const_iterator i = s.begin(), end = s.end(); i != end; ++i) {
    unsigned char c = *i;
    if (' ' <= c and c <= '~' and c != '\\' and c != '"') {
      out += c;
    }
    else {
      out += '\\';
      switch(c) {
      case '"':  out += '"';  break;
      case '\\': out += '\\'; break;
      case '\t': out += 't';  break;
      case '\r': out += 'r';  break;
      case '\n': out += 'n';  break;
      default:
        char const* const hexdig = "0123456789ABCDEF";
        out += 'x';
        out += hexdig[c >> 4];
        out += hexdig[c & 0xF];
      }
    }
  }
  out += '"';
  return out;
}

// msdfgen::FreetypeHandle

void fx_Font::InitRuntime()
{
    if (!FTRuntime)
    {
        // msdfgen::FreetypeHandle *ft = msdfgen:?S:initializeFreetype();
        if (FT_Init_FreeType(&FTRuntime))
        {
            std::cout << "Font.cpp: Failed to load FreeType Runtime\n";
        }
    }
}

fx_Image fx_Font::RenderChar(uint32_t GlyphIndex, unsigned int RenderMode)
{
    fx_Image Glyph;
    if (FT_Load_Glyph((FT_Face)m_FontFace, GlyphIndex, FT_LOAD_DEFAULT))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }
    if (FT_Render_Glyph(((FT_Face)m_FontFace)->glyph, (FT_Render_Mode_)RenderMode))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }

    Glyph.Component = 1;
    Glyph.Width = ((FT_Face)m_FontFace)->glyph->bitmap.width;
    Glyph.Height = ((FT_Face)m_FontFace)->glyph->bitmap.rows;
    Glyph.Data = std::vector<unsigned char>(((FT_Face)m_FontFace)->glyph->bitmap.buffer, ((FT_Face)m_FontFace)->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
    return Glyph;
}


// void fx_Font::CreateAtlas()
// {
    // unsigned int MaxGlyph = CHAR_MAX;
    // fx_Image BlankImage;
    // BlankImage.Width = 0;
    // BlankImage.Height = 0;
    // BlankImage.Component = 1;
    // std::vector<fx_Image> Characters;
    // Characters.resize(MaxGlyph);
    // std::fill(Characters.begin(), Characters.end(), BlankImage);
    // // Characters[0] = RenderChar(0, FT_Get_Char_Index((FT_Face)m_FontFace, 0));
    // unsigned int GlyphIndex = 0;
    // unsigned int Charcode = FT_Get_First_Char((FT_Face)m_FontFace, &GlyphIndex);

    // while ( GlyphIndex != 0 )
    // {
    //     Characters[Charcode] = RenderChar(Charcode, GlyphIndex);
    //     Charcode = FT_Get_Next_Char( (FT_Face)m_FontFace, Charcode, &GlyphIndex );
    //     if (Charcode > MaxGlyph)
    //     {
    //         break;
    //     }
    // }
    // m_CharAtlas = fx_Atlas::PackImages(Characters);
// }



fx_Font::fx_Font(std::string FontPath)
{
    InitRuntime();

    if (FT_New_Face(FTRuntime, FontPath.c_str(), 0, (FT_Face*)&m_FontFace))
    {
        std::cout << "Font.cpp: Failed to load FT_Face\n";
    }

    FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, FONT_SIZE_PIXEL);  

    // m_FontFace2 = msdfgen::adoptFreetypeFont((FT_Face)m_FontFace);
    FontCount++;
    m_FontId = FontCount;

    // CreateAtlas();
}

fx_Font::fx_Font(std::vector<uint8_t> Buffer)
{
    InitRuntime();
    m_FontCache = Buffer;

    if (FT_New_Memory_Face(FTRuntime, m_FontCache.data(), m_FontCache.size(), 0, (FT_Face*)&m_FontFace))
    {
        std::cout << "Font.cpp: Failed to load FT_Face\n";
    }
    FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, FONT_SIZE_PIXEL);  

    // m_FontFace2 = msdfgen::adoptFreetypeFont((FT_Face)m_FontFace);
    FontCount++;
    m_FontId = FontCount;

    // CreateAtlas();
}

fx_Font::~fx_Font()
{
    FT_Done_Face((FT_Face)m_FontFace);
    m_FontFace = NULL;
    // msdfgen::destroyFont((msdfgen::FontHandle*)m_FontFace2);

    FontCount--;

    if (!FontCount)
    {
        if (FT_Done_FreeType(FTRuntime))
        {
            std::cout << "Font.cpp: Failed to load delete FT_LIbrary\n";
        }
        FTRuntime = NULL;
    }
}

void fx_Font::RenderFont()
{
    if (!m_Atlas)
    {
        return;
    }
    
    const std::string FontId = GetFontID();
    const unsigned int MaxGlyph = CHAR_MAX;

    unsigned int GlyphIndex = 0;
    unsigned int Charcode = FT_Get_First_Char((FT_Face)m_FontFace, &GlyphIndex);

    while ( GlyphIndex != 0 )
    {
        FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, FONT_SIZE_PIXEL);
        m_Atlas->ImagesList[FontId + "_SDF_" + std::to_string(Charcode)].Image = RenderChar(GlyphIndex, FT_RENDER_MODE_SDF);

        // for (auto x : BitmapPixels)
        for (int i = 1; i <= FONT_SIZE_PIXEL_SMALL; i++)
        {
            FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, i);
            m_Atlas->ImagesList[FontId + "_BMP_" + std::to_string(i) + "_" + std::to_string(Charcode)].Image = RenderChar(GlyphIndex, FT_RENDER_MODE_NORMAL);
        }

        Charcode = FT_Get_Next_Char( (FT_Face)m_FontFace, Charcode, &GlyphIndex );
        if (Charcode > MaxGlyph)
        {
            break;
        }
    }
}

inline float FtFloatToFloat(int32_t input)
{
    return input / 64.0f;
}

inline std::string v3tostr(glm::vec3 a)
{
    return "{" + std::to_string(a.x) + "," + std::to_string(a.y) + "," + std::to_string(a.z) + "}";
}

fx_Text::fx_Text(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour, glm::vec4 Background)
{
    SetPosition(Pos);
    SetColour(Colour);
    SetText(Text);
    SetFont(Font);
    SetLineHeight(LineHeight);
}

void fx_Text::SetText(std::string Text)
{
    if (m_Text != Text)
    {
        if (m_Text.size() == Text.size())
        {
            m_FlagUpdateMesh |= true;
        }
        else
        {
            m_FlagUpdateObject |= true; 
        }
        
    }
    m_Text = Text;
}

void fx_Text::Update()
{   
    if (m_FlagUpdateObject)
    {
        for (auto x : m_TextObj)
        {
            delete x;
        }
        m_Objects.clear();
        m_TextObj.clear();
    }
    
    unsigned int PixelLineHeight = std::round(m_LineHeight * m_PixelDensity);
    unsigned int PixSize = 0;
    std::string TextId;
    if (PixelLineHeight > FONT_SIZE_PIXEL_SMALL || m_PixelDensity < 0)
    {
        TextId = m_Font->GetFontID() + "_SDF";
        PixSize = FONT_SIZE_PIXEL;
        // m_Font->SetSize(FONT_SIZE_PIXEL);
    }
    else
    {
        TextId = m_Font->GetFontID() + "_BMP_" + std::to_string(PixSize);
        PixSize = PixelLineHeight;
    }

    FT_Set_Pixel_Sizes((FT_Face)(m_Font->m_FontFace), 0, PixSize);

    // float Scalingfactor = m_LineHeight / (FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.height));
    // m_Ascender = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.ascender) * Scalingfactor;
    // m_Descender = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.descender) * Scalingfactor;
    
    std::vector<glm::vec4> Layout = fx_Text::GetTextLayout(m_Text, m_LineHeight, m_Kerning, m_Font);
    m_Cube = {Layout[0].z, Layout[0].w, 1.0f};

    for (unsigned int i = 0; i < m_Text.size(); i++)
    {
        glm::vec3 FontPos = {Layout[i+1].x, Layout[i+1].y, 0.0f};
        glm::vec3 OffsetPos = { m_Anchor.x * m_Cube.x, m_Anchor.y * m_Cube.y, 0};
        glm::vec3 GlyphPos = glm::toMat3(m_Quat) * (FontPos - OffsetPos);
        
        std::string CharID = TextId + "_" + std::to_string(m_Text[i]);
        fx_UV CharTexturePos = m_Font->m_Atlas->ImagesList[CharID].UV;

        float CharWidth = (CharTexturePos.X2 - CharTexturePos.X1) / (CharTexturePos.Y2 - CharTexturePos.Y1);
        fx_SDF *Character;
        if (m_FlagUpdateObject)
        {
            Character = new fx_SDF(m_Position + GlyphPos, glm::vec2(Layout[i+1].w * CharWidth  ,Layout[i+1].w), CharTexturePos, m_Colour);
            m_TextObj.push_back(Character);
            m_Objects.insert(Character);
        }
        else
        {
            Character = m_TextObj[i];
        }
        Character->SetAnchor({0.0f,0.0f,0.0f});
        // std::cout << v3tostr(m_Position) + " " + v3tostr(PixelSnap(m_Position, m_PixelDensity)) + "\n";
        Character->SetPosition(PixelSnap(m_Position + GlyphPos, m_PixelDensity));
        Character->SetCube(PixelSnap(glm::vec3(Layout[i+1].w * CharWidth ,Layout[i+1].w, 1.0f), m_PixelDensity));
        Character->SetUV(CharTexturePos);
        Character->SetColour(m_Colour);
        // Character->SetGlowTreshold(m_GlowThreshold);
        // Character->SetGlowColour(m_GlowColour);
        // Character->SetOutlineTreshold(m_OutlineThreshold);
        // Character->SetOutlineTreshold(m_OutlineColour);
        Character->SetQuat(m_Quat);
    }

}

std::vector<glm::vec4> fx_Text::GetTextLayout(std::string Text, float LineHeight, float Kerning, fx_Font *Font)
{
    std::vector<glm::vec4> Result;
    Result.resize(Text.size()+1);
    // glm::vec3 Scale = {0, m_LineHeight, 1};
    float x = 0;

    float Scalingfactor = LineHeight / (FtFloatToFloat(((FT_Face)Font->m_FontFace)->size->metrics.height));

    uint32_t previous    = 0;

    float Decender = FtFloatToFloat(((FT_Face)Font->m_FontFace)->size->metrics.descender);


    for(unsigned int i = 0; i < Text.size(); i ++)
    {
        uint32_t glyph_index = FT_Get_Char_Index( (FT_Face)Font->m_FontFace, Text[i] );
        FT_Load_Glyph((FT_Face)Font->m_FontFace, glyph_index, FT_LOAD_DEFAULT);
        float Height = FtFloatToFloat(((FT_Face)Font->m_FontFace)->glyph->metrics.height);
        // float Width = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.width);
        float Advance = FtFloatToFloat(((FT_Face)Font->m_FontFace)->glyph->metrics.horiAdvance);
        float BearingX = FtFloatToFloat(((FT_Face)Font->m_FontFace)->glyph->metrics.horiBearingX);
        float BearingY = FtFloatToFloat(((FT_Face)Font->m_FontFace)->glyph->metrics.horiBearingY);
        x += Kerning;

        FT_Vector  delta;

        FT_Get_Kerning( (FT_Face)Font->m_FontFace, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
        x += FtFloatToFloat(delta.x);

        Result[i+1] = {(x + BearingX) * Scalingfactor , (BearingY - Height - Decender) * Scalingfactor , 0, (Height)* Scalingfactor};
        
        x += Advance;
        previous = glyph_index;
    }

    Result[0] = {0,0, x * Scalingfactor, LineHeight};

    return Result;
}

std::vector<std::string> fx_TextBox::Tokenize(std::string Text)
{
    std::vector<std::string> Result;
    unsigned int pos = 0;
    // std::replace( Text.begin(), Text.end(), 'x', 'y'); // replace all 'x' to 'y'
    Text.erase(std::remove(Text.begin(), Text.end(), '\r'), Text.end());

    while (pos < Text.size())
    {
        size_t  NearSpace = Text.find(" ", pos);
        size_t  NearNL = Text.find("\n", pos);

        if (NearSpace == std::string::npos && NearSpace == std::string::npos)
        {
            Result.push_back(Text.substr(pos));
            pos = Text.size();
            break;
        }

        size_t  Cut = ((NearNL < NearSpace)? NearNL:NearSpace) + 1;

        std::string Token = Text.substr(pos, Cut-pos);
        
        // std::cout << Escape(Token) << "\n";
        Result.push_back(Token);
        if (Cut < pos)
        {
            pos = Text.size();
            break;
        }
        pos = Cut;
    }
    return Result;
}

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t\n")
{


    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - 0 + 1;

    return str.substr(0, strRange);
}

std::vector<std::string> SplitText(float Width, float LineHeight, float Kerning, fx_Font *Font,std::string Text)
{
    std::vector<std::string> Result;
    float TextWidth = fx_Text::GetTextLayout(Text, LineHeight, Kerning, Font)[0].z;
    if (TextWidth < Width)
    {
        Result.push_back(Text);
        return Result;
    }
    while(TextWidth >= Width)
    {
        unsigned int low = 0;
        unsigned int high = Text.size() - 1;
        while ((high-low)>1)
        {
            unsigned int mid = low + ((high - low) / 2);
            if (fx_Text::GetTextLayout(trim(Text.substr(0,mid+1)), LineHeight, Kerning, Font)[0].z > Width)
            {
                high = mid;
            }
            else
            {
                low = mid;
            }
        }
        Result.push_back(Text.substr(0,high));
        Text = Text.substr(high);

        TextWidth = fx_Text::GetTextLayout(Text, LineHeight, Kerning, Font)[0].z;
        if (TextWidth < Width)
        {
            Result.push_back(Text);
            return Result;
        }
    }
    return Result;
}

std::vector<std::string> fx_TextBox::Box(float Width, float LineHeight, float Kerning, fx_Font *Font,std::string Text)
{
    std::vector<std::string> Result;
    std::vector<std::string> Token = fx_TextBox::Tokenize(Text);
    std::string Buff = "";
    for (unsigned int i = 0; i < Token.size(); i++)
    {
        if (fx_Text::GetTextLayout(Token[i], LineHeight, Kerning, Font)[0].z > Width)
        {
            if (Buff.size() != 0)
            {
                Result.push_back(trim(Buff));
            }
            std::vector<std::string> split = SplitText(Width, LineHeight, Kerning, Font, Token[i]);
            Result.insert(Result.end(), split.begin(), split.end()-1);
            Buff = split.back();
            if (Buff.back() == '\n' && Buff.size() > 1)
            {
                Result.push_back(trim(Buff));
                Buff = "";
            }
            else if (Buff == "\n")
            {
                Buff = "";
            }
            continue;
        }
        if (Token[i].back() == '\n')
        {
            Result.push_back(trim(Buff + Token[i]));
            Buff = "";
            continue;
        }
        if (fx_Text::GetTextLayout(Buff + Token[i], LineHeight, Kerning, Font)[0].z > Width)
        {
            Result.push_back(trim(Buff));
            Buff = Token[i];
            continue;
        }
        Buff += Token[i];
    }
    Result.push_back(Buff);
    // Result = SplitText(Width, LineHeight, Kerning, Font, Text);
    // for (auto x : Result)
    // {
    //     std::cout << Escape(x) << "\n";
    // }
    return Result;
}

fx_TextBox::fx_TextBox(glm::vec3 Pos, float LineHeight, float Width, fx_Font *Font, std::string Text, glm::vec4 Colour, glm::vec4 Background)
{
    SetPosition(Pos);
    SetColour(Colour);
    SetFont(Font);
    SetLineHeight(LineHeight);
    m_Cube.x = Width;
    SetText(Text);

    m_Bg = new fx_Quad(Pos, {1,1},Background );
    SetBackgroundColour(Background);

    m_Objects = {m_Bg};
}

void fx_TextBox::SetText(std::string Text)
{
    if (m_Text == Text)
    {
        return;
    }
    m_Text = Text;
    std::vector<std::string> Lines = fx_TextBox::Box(m_Cube.x, m_LineHeight, m_Kerning, m_Font, m_Text);
    if (m_LineText.size() == Lines.size())
    {
        m_FlagUpdateMesh |= true;
    }
    else
    {
        m_FlagUpdateObject |= true;
    }
    m_LineText = Lines;

}

void fx_TextBox::Update()
{   
    // float Scalingfactor = m_LineHeight / (FtFloatToFloat(((FT_Face)m_Font->GetInternalFontFace())->size->metrics.height));
    // float Ascender = FtFloatToFloat(((FT_Face)m_Font->GetInternalFontFace())->size->metrics.ascender) * Scalingfactor;
    // float Descender = FtFloatToFloat(((FT_Face)m_Font->GetInternalFontFace())->size->metrics.descender) * Scalingfactor;
    
    
    if (m_FlagUpdateObject)
    {
        for (auto x : m_Lines)
        {
            delete x;
        }
        m_Lines.clear();
        m_Objects.clear();
        m_Objects.insert(m_Bg);
        
        
        for (auto x : m_LineText)
        {
            fx_Text *Line = new fx_Text({m_Position.x, m_Position.y, m_Position.z}, m_LineHeight, m_Font, x);
            
            m_Objects.insert(Line);
            m_Lines.push_back(Line);
        }

    }
    
    float height = m_LineHeight + (m_LineHeight * m_LineSpacing * (m_Lines.size() - 1));
    m_Cube.y = height;
    glm::vec3 Offset = { (m_Anchor.x - m_Align) * m_Cube.x, m_Anchor.y * m_Cube.y, 0};
    
    
    float Y = m_Position.y + height;
    unsigned int i = 0;
    for (auto x : m_Lines)
    {
        glm::vec3 Pos = m_Position;
        Pos.y = Y;
        Pos.z++;
        x->SetText(m_LineText[i]);
        x->SetPosition(Pos - Offset);
        x->SetColour(m_Colour);
        x->SetAnchor({m_Align,1.0f,0.0f});
        x->SetFont(m_Font);
        glm::vec3 Size = x->GetCube();
        Size.x = m_Cube.x;
        x->SetCube(Size);
        x->SetPixelDensity(m_PixelDensity);
        Y -= m_LineHeight * m_LineSpacing;
        i++;
    }

    m_Bg->SetAnchor(m_Anchor);
    m_Bg->SetCube(m_Cube);
    m_Bg->SetPosition(m_Position);


}