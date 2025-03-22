#include "Font.hpp"

#include <string>
#include <vector>
#include <exception>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Resource.hpp"

#define FONT_SIZE_PIXEL 64

static FT_Library FTRuntime = NULL;
static uint32_t FontCount = 0;

void fx_Font::InitRuntime()
{
    if (!FTRuntime)
    {
        if (FT_Init_FreeType(&FTRuntime))
        {
            std::cout << "Font.cpp: Failed to load FreeType Runtime\n";
        }
    }
}

fx_Image fx_Font::RenderChar(uint32_t Code)
{
    fx_Image Glyph;
    if (FT_Load_Char((FT_Face)m_FontFace, Code, FT_LOAD_DEFAULT))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }
    if (FT_Render_Glyph(((FT_Face)m_FontFace)->glyph, FT_RENDER_MODE_SDF))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    }

    Glyph.Component = 1;
    Glyph.Width = ((FT_Face)m_FontFace)->glyph->bitmap.width;
    Glyph.Height = ((FT_Face)m_FontFace)->glyph->bitmap.rows;
    Glyph.Data = std::vector<unsigned char>(((FT_Face)m_FontFace)->glyph->bitmap.buffer, ((FT_Face)m_FontFace)->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
    return Glyph;
}


void fx_Font::CreateAtlas()
{
    unsigned int MaxGlyph = CHAR_MAX;
    fx_Image BlankImage;
    BlankImage.Width = 0;
    BlankImage.Height = 0;
    BlankImage.Component = 1;
    std::vector<fx_Image> Characters;
    Characters.resize(MaxGlyph);
    std::fill(Characters.begin(), Characters.end(), BlankImage);
    Characters[0] = RenderChar(0);
    unsigned int GlyphIndex = 0;
    unsigned int Charcode = FT_Get_First_Char((FT_Face)m_FontFace, &GlyphIndex);

    while ( GlyphIndex != 0 )
    {
        Characters[Charcode] = RenderChar(Charcode);
        Charcode = FT_Get_Next_Char( (FT_Face)m_FontFace, Charcode, &GlyphIndex );
        if (Charcode > MaxGlyph)
        {
            break;
        }
    }
    m_CharAtlas = fx_Atlas::PackImages(Characters);
}


fx_Font::fx_Font(std::string FontPath)
{
    InitRuntime();

    if (FT_New_Face(FTRuntime, FontPath.c_str(), 0, (FT_Face*)&m_FontFace))
    {
        std::cout << "Font.cpp: Failed to load FT_Face\n";
    }

    FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, FONT_SIZE_PIXEL);  

    FontCount++;

    CreateAtlas();
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

    FontCount++;

    CreateAtlas();
}

fx_Font::~fx_Font()
{
    FT_Done_Face((FT_Face)m_FontFace);
    m_FontFace = NULL;

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

inline float FtFloatToFloat(int32_t input)
{
    return input / 64.0f;
}

fx_Text::fx_Text(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour, glm::vec4 Background)
{
    SetPosition(Pos);
    SetColour(Colour);
    SetText(Text);
    SetFont(Font);
    SetLineHeight(LineHeight);
    m_Objects = {};
}

void fx_Text::Update()
{   
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    if (m_FlagUpdateObject)
    {
        for (auto x : m_Objects)
        {
            delete x;
        }
        m_Objects.clear();
        m_Objects.resize(0);
        m_Objects.reserve(m_Text.size());
    }

    float Scalingfactor = m_LineHeight / (FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.height));
    m_Ascender = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.ascender) * Scalingfactor;
    m_Descender = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.descender) * Scalingfactor;


    std::vector<glm::vec4> Layout = GetTextLayout(m_Text);
    fx_Atlas Atlas = m_Font->GetAtlas();
    m_Cube = {Layout[0].z, Layout[0].w, 1.0f};
    for (unsigned int i = 0; i < m_Text.size(); i++)
    {
        glm::vec3 FontPos = {Layout[i+1].x, Layout[i+1].y, 0.0f};
        glm::vec3 OffsetPos = { m_Anchor.x * m_Cube.x, m_Anchor.y * m_Cube.y, 0};
        glm::vec3 GlyphPos = glm::toMat3(m_Quat) * (FontPos - OffsetPos);
        fx_UV CharTexturePos;
        if (Atlas.Coord[m_Text[i]].H * Atlas.Coord[m_Text[i]].W == 0)
        {
            CharTexturePos = fx_Atlas::GetUV( 0, Atlas, 5);
        }
        else
        {
            CharTexturePos = fx_Atlas::GetUV( m_Text[i], Atlas, 5);
        }

        float CharWidth = (CharTexturePos.X2 - CharTexturePos.X1) / (CharTexturePos.Y2 - CharTexturePos.Y1);
        fx_SDF *Character;
        if (m_FlagUpdateObject)
        {
            Character = new fx_SDF(m_Position + GlyphPos, glm::vec2(Layout[i+1].w * CharWidth  ,Layout[i+1].w), CharTexturePos, m_Colour);
            m_Objects.push_back(Character);
        }
        else
        {
            Character = (fx_SDF*)m_Objects[i];
        }
        Character->SetAnchor({0.0f,0.0f,0.0f});
        Character->SetPosition(m_Position + GlyphPos);
        Character->SetCube(glm::vec3(Layout[i+1].w * CharWidth ,Layout[i+1].w, 1.0f));
        Character->SetUV(CharTexturePos);
        Character->SetColour(m_Colour);
        Character->SetGlowTreshold(m_GlowThreshold);
        Character->SetGlowColour(m_GlowColour);
        Character->SetOutlineTreshold(m_OutlineThreshold);
        Character->SetOutlineTreshold(m_OutlineColour);
        Character->SetQuat(m_Quat);
    }

}

std::vector<glm::vec4> fx_Text::GetTextLayout(std::string Text)
{
    std::vector<glm::vec4> Result;
    Result.resize(Text.size()+1);
    glm::vec3 Scale = {0, m_LineHeight, 1};
    float x = 0;

    float Scalingfactor = m_LineHeight / (FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.height));

    uint32_t previous    = 0;

    float Decender = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.descender);


    for(unsigned int i = 0; i < Text.size(); i ++)
    {
        uint32_t glyph_index = FT_Get_Char_Index( (FT_Face)m_Font->m_FontFace, Text[i] );
        FT_Load_Glyph((FT_Face)m_Font->m_FontFace, glyph_index, FT_LOAD_DEFAULT);
        float Height = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.height);
        // float Width = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.width);
        float Advance = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiAdvance);
        float BearingX = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiBearingX);
        float BearingY = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiBearingY);
        x += m_Kerning;

        FT_Vector  delta;

        FT_Get_Kerning( (FT_Face)m_Font->m_FontFace, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
        x += FtFloatToFloat(delta.x);

        Result[i+1] = {(x + BearingX) * Scalingfactor , (BearingY - Height - Decender) * Scalingfactor , 0, (Height)* Scalingfactor};
        
        x += Advance;
        previous = glyph_index;
    }

    Result[0] = {0,0, x * Scalingfactor, m_LineHeight};

    return Result;
}