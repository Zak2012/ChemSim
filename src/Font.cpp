#include "Font.hpp"

#include <string>
#include <vector>
#include <exception>
#include <algorithm>

#include <hb/hb.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Resource.hpp"

struct fx_Font_Library_s
{
    FT_Library FTLibrary;
};

fx_Font_Library fx_Load_Lib()
{
    fx_Font_Library Lib = new fx_Font_Library_s;
    if (FT_Init_FreeType(&Lib->FTLibrary))
    {
        throw std::runtime_error("Font.cpp: Failed to load FT_LIbrary");
    }
    return Lib;
}

void fx_Delete_Lib(fx_Font_Library Lib)
{
    if (FT_Done_FreeType(Lib->FTLibrary))
    {
        throw std::runtime_error("Font.cpp: Failed to delete FT_LIbrary");
    }
}

struct fx_Face_s
{
    FT_Face FTFace;
    hb_blob_t *HBBlob;
    hb_face_t *HBFace;
    hb_font_t *HBFont;
    bool SDF = true;
    int Size = 64;
};

fx_Face fx_Load_Face(fx_Font_Library Lib, std::string FileName)
{
    fx_Face Face = new fx_Face_s;
    int a = FT_New_Face(Lib->FTLibrary, FileName.c_str(), 0, &Face->FTFace);
    if (a)
    {
        throw std::runtime_error("Font.cpp: Failed to load FT_Face");
    }

    Face->HBBlob = hb_blob_create_from_file_or_fail(FileName.c_str());

    if (Face->HBBlob == NULL)
    {
        throw std::runtime_error("Font.cpp: Failed to load hb_blob_t");
    }

    FT_Set_Pixel_Sizes(Face->FTFace, 0, Face->Size);  

    Face->HBFace = hb_face_create(Face->HBBlob, 0);
    Face->HBFont = hb_font_create(Face->HBFace);
    hb_font_set_scale(Face->HBFont, Face->Size, Face->Size);
    return Face;
}

int fx_Get_Size(fx_Face Face)
{
    return Face->Size;
}
void fx_Set_Size(fx_Face Face, int Size)
{
    Face->Size = Size;
    FT_Set_Pixel_Sizes(Face->FTFace, 0, Face->Size);  

    Face->HBFace = hb_face_create(Face->HBBlob, 0);
    Face->HBFont = hb_font_create(Face->HBFace);
    hb_font_set_scale(Face->HBFont, Face->Size, Face->Size);
}

bool fx_Get_SDF(fx_Face Face)
{
    return Face->SDF;
}
void fx_Set_SDF(fx_Face Face, bool SDF)
{
    Face->SDF = SDF;
}

void fx_Delete_Face(fx_Face Face)
{
    FT_Done_Face(Face->FTFace);

    hb_font_destroy(Face->HBFont);
    hb_face_destroy(Face->HBFace);
    hb_blob_destroy(Face->HBBlob);
    delete Face;
}

fx_Atlas fx_GenerateAtlas(fx_Face Face, bool IsWideChar)
{
    unsigned int MaxGlyph = IsWideChar?USHRT_MAX:CHAR_MAX;
    fx_Image BlankImage;
    BlankImage.Width = 0;
    BlankImage.Height = 0;
    BlankImage.Component = 1;
    std::vector<fx_Image> Characters;
    Characters.resize(MaxGlyph);
    std::fill(Characters.begin(), Characters.end(), BlankImage);
    {
        fx_Image Glyph;
        if (FT_Load_Char(Face->FTFace, 0, FT_LOAD_DEFAULT))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        }
        if (FT_Render_Glyph(Face->FTFace->glyph, Face->SDF?FT_RENDER_MODE_SDF:FT_RENDER_MODE_NORMAL))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        }

        Glyph.Component = 1;
        Glyph.Width = Face->FTFace->glyph->bitmap.width;
        Glyph.Height = Face->FTFace->glyph->bitmap.rows;
        Glyph.Data = std::vector<unsigned char>(Face->FTFace->glyph->bitmap.buffer, Face->FTFace->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
        fx_Image::FlipImageVert(Glyph);
        Characters[0] = fx_Image::PadImage(Glyph, 5, 5);
    }
    unsigned int GlyphIndex = 0;
    unsigned int Charcode = FT_Get_First_Char(Face->FTFace, &GlyphIndex);

    while ( GlyphIndex != 0 )
    {
        fx_Image Glyph;
        if (FT_Load_Char(Face->FTFace, Charcode, FT_LOAD_DEFAULT))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        if (FT_Render_Glyph(Face->FTFace->glyph, Face->SDF?FT_RENDER_MODE_SDF:FT_RENDER_MODE_NORMAL))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        Glyph.Component = 1;
        Glyph.Width = Face->FTFace->glyph->bitmap.width;
        Glyph.Height = Face->FTFace->glyph->bitmap.rows;
        Glyph.Data = std::vector<unsigned char>(Face->FTFace->glyph->bitmap.buffer, Face->FTFace->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
        Characters[Charcode] = fx_Image::PadImage(Glyph, 5, 5);
        Charcode = FT_Get_Next_Char( Face->FTFace, Charcode, &GlyphIndex );
        if (Charcode > MaxGlyph)
        {
            break;
        }
    }
    fx_Atlas Result = fx_Atlas::PackImages(Characters);
    return Result;
}

void fx_Load_Font(fx_Atlas *InAtlas, fx_Face InFace, std::string Filename, unsigned int Index, bool Wide)
{
    if (std::filesystem::exists(Filename))
    {
        *InAtlas = fx_Atlas::ReadAtlas(Filename);
    }
    else
    {
        *InAtlas = fx_GenerateAtlas(InFace, Wide);
        fx_Atlas::SaveAtlas(*InAtlas, Filename);
    }
}

// fx_WText::fx_WText(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::wstring Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color)
// {
//     m_Info.m_Size = glm::vec3(Size, 1.0f);
//     m_Info.m_Position = Pos;
//     m_Info.m_Color = Color;
//     m_Objects = {};
//     m_Text = Text;
//     m_Size = {0, m_Info.m_Size.y, 1};
//     m_FontAtlas = Atlas;
//     m_FontOffset = FontOffset;
//     m_Face = Face;
//     m_Complex = true;

//     std::vector<fx_CharInfo> Layout = GetTextLayout(m_Text);
//     for (unsigned int i = 0; i < Layout.size(); i++)
//     {
//         glm::vec3 GlyphPos = {Layout[i].X, Layout[i].Y, 0.0f};
//         fx_UV CharTexturePos;
//         if (m_FontAtlas.Coord[m_FontOffset + m_Text[i]].H * m_FontAtlas.Coord[m_FontOffset + m_Text[i]].W == 0)
//         {
//             CharTexturePos = fx_Atlas::GetUV( m_FontOffset, m_FontAtlas, 5, 5);
//         }
//         else
//         {
//             CharTexturePos = fx_Atlas::GetUV( m_FontOffset + m_Text[i], m_FontAtlas, 5, 5);
//         }
//         if (m_Face->SDF)
//         {
//             fx_SDF *Character = new fx_SDF(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
//             m_Objects.push_back(Character);
//         }
//         else
//         {
//             fx_Sprite *Character = new fx_Sprite(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
//             m_Objects.push_back(Character);
//         }
//     }

// }

// std::vector<fx_CharInfo> fx_WText::GetTextLayout(std::wstring Text)
// {
//     std::vector<fx_CharInfo> Result;
//     Result.reserve(Text.size());
//     m_Size = {0, m_Info.m_Size.y, 1};
//     float x = 0;

//     hb_buffer_t *HBBuffer;
//     HBBuffer = hb_buffer_create();
//     hb_buffer_add_utf16(HBBuffer, (uint16_t *)Text.c_str(), -1, 0, -1);

//     hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);

//     hb_shape(m_Face->HBFont, HBBuffer, NULL, 0);

//     unsigned int glyph_count;
//     // hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(HBBuffer, &glyph_count);
//     hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(HBBuffer, &glyph_count);

//     float FontHeight = (float)((m_Face->FTFace->size->metrics.ascender - m_Face->FTFace->size->metrics.descender) >> 6) + 8.0f;


//     for(unsigned int i = 0; i < Text.size(); i ++)
//     {
//         float Advance = ((float)(glyph_pos[i].x_advance - 4) / FontHeight) * m_Info.m_Size.x;
//         m_Size.x += m_Kerning;
//         if (i + 1 == Text.size())
//         {
//             FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
//             float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.x;
//             m_Size.x += Width;
//         }
//         else
//         {
//         m_Size.x += Advance;
//         }
//     }
//     m_Size -= (8 /FontHeight) * m_Info.m_Size.x;
//     for(unsigned int i = 0; i < Text.size(); i ++)
//     {
//         FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
//         float Height = ((float)((m_Face->FTFace->glyph->metrics.height >> 6) + 8)/ FontHeight) * m_Info.m_Size.y;
//         float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.x;
//         float Advance = ((float)(glyph_pos[i].x_advance - 4) / FontHeight) * m_Info.m_Size.x;
//         float BearingX = ((float)((m_Face->FTFace->glyph->metrics.horiBearingX >> 6) - 4)/ FontHeight) * m_Info.m_Size.x;
//         float BearingY = ((float)((m_Face->FTFace->glyph->metrics.horiBearingY >> 6) - 4)/ FontHeight) * m_Info.m_Size.y;
//         float Baseline = ((float)(((-m_Face->FTFace->size->metrics.descender >> 6) + 8) / FontHeight)) * m_Info.m_Size.y;
//         x += m_Kerning;

//         Result.push_back({x + BearingX, Baseline + (BearingY - Height), Width, Height});
//         x += Advance;
//     }

//     hb_buffer_destroy(HBBuffer);
//     return Result;
// }


// void fx_WText::Update()
// {
//     for (auto &x : m_Objects)
//     {
//         delete x;
//     }
//     m_Objects.clear();
//     m_Objects.resize(0);
//     m_Objects.reserve(m_Text.size());

//     m_Size = {0, m_Info.m_Size.y, 1};

//     std::vector<fx_CharInfo> Layout = GetTextLayout(m_Text);
//     for (unsigned int i = 0; i < Layout.size(); i++)
//     {
//         glm::vec3 FontPos = {Layout[i].X, Layout[i].Y, 0.0f};
//         glm::vec3 OffsetPos = { m_Info.m_Anchor.x * m_Size.x, m_Info.m_Anchor.y * m_Size.y, 0};
//         glm::vec3 GlyphPos = glm::toMat3(m_Info.m_Rotation) * (FontPos - OffsetPos);
//         fx_UV CharTexturePos;
//         if (m_FontAtlas.Coord[m_FontOffset + m_Text[i]].H * m_FontAtlas.Coord[m_FontOffset + m_Text[i]].W == 0)
//         {
//             CharTexturePos = fx_Atlas::GetUV( m_FontOffset, m_FontAtlas, 5, 5);
//         }
//         else
//         {
//             CharTexturePos = fx_Atlas::GetUV( m_FontOffset + m_Text[i], m_FontAtlas, 5, 5);
//         }
//         if (m_Face->SDF)
//         {
//             fx_SDF *Character = new fx_SDF(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
//             Character->m_GlowThreshold = m_GlowThreshold;
//             Character->m_GlowColor = m_GlowColor;
//             Character->m_OutlineThreshold = m_OutlineThreshold;
//             Character->m_OutlineColor = m_OutlineColor;
//             Character->m_Info.m_Rotation = m_Info.m_Rotation;
//             Character->Update();
//             m_Objects.push_back(Character);
//         }
//         else
//         {
//             fx_Sprite *Character = new fx_Sprite(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
//             Character->m_Info.m_Rotation = m_Info.m_Rotation;
//             Character->Update();
//             m_Objects.push_back(Character);
//         }
//     }
//     for (auto x : m_Objects)
//     {
//         x->Update();
//     }
//     GenerateMesh();
// }

fx_Text::fx_Text(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::string Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color)
{
    m_Info.m_Size = glm::vec3(0.0f, Size.y, 1.0f);
    m_Info.m_Position = Pos;
    m_Info.m_Color = Color;
    m_Objects = {};
    m_Text = Text;
    m_FontAtlas = Atlas;
    m_FontOffset = FontOffset;
    m_Face = Face;
    m_Drawable = true;
    m_Complex = true;

    std::vector<fx_CharInfo> Layout = GetTextLayout(m_Text);
    for (unsigned int i = 0; i < Layout.size(); i++)
    {
        glm::vec3 GlyphPos = {Layout[i].X, Layout[i].Y, 0.0f};
        fx_UV CharTexturePos;
        if (m_FontAtlas.Coord[m_FontOffset + m_Text[i]].H * m_FontAtlas.Coord[m_FontOffset + m_Text[i]].W == 0)
        {
            CharTexturePos = fx_Atlas::GetUV( m_FontOffset, m_FontAtlas, 5, 5);
        }
        else
        {
            CharTexturePos = fx_Atlas::GetUV( m_FontOffset + m_Text[i], m_FontAtlas, 5, 5);
        }
        
        if (m_Face->SDF)
        {
            fx_SDF *Character = new fx_SDF(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
            m_Objects.push_back(Character);
        }
        else
        {
            fx_Sprite *Character = new fx_Sprite(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
            m_Objects.push_back(Character);
        }
    }

}

std::vector<fx_CharInfo> fx_Text::GetTextLayout(std::string Text)
{
    std::vector<fx_CharInfo> Result;
    Result.reserve(Text.size());
    glm::vec3 Scale = {0, m_Info.m_Size.y, 1};
    float x = 0;

    hb_buffer_t *HBBuffer;
    HBBuffer = hb_buffer_create();
    hb_buffer_add_utf8(HBBuffer, Text.c_str(), -1, 0, -1);

    hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);

    hb_shape(m_Face->HBFont, HBBuffer, NULL, 0);

    unsigned int glyph_count;
    // hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(HBBuffer, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(HBBuffer, &glyph_count);

    float FontHeight = (float)((m_Face->FTFace->size->metrics.ascender - m_Face->FTFace->size->metrics.descender) >> 6) + 8.0f;


    for(unsigned int i = 0; i < Text.size(); i ++)
    {
        float Advance = ((float)(glyph_pos[i].x_advance - 4) / FontHeight) * m_Info.m_Size.x;
        Scale.x += m_Kerning;
        if (i + 1 == Text.size())
        {
            FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
            float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.x;
            Scale.x += Width;
        }
        else
        {
        Scale.x += Advance;
        }
    }
    Scale -= (8 /FontHeight) * m_Info.m_Size.y;
    for(unsigned int i = 0; i < Text.size(); i ++)
    {
        FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
        float Height = ((float)((m_Face->FTFace->glyph->metrics.height >> 6) + 8)/ FontHeight) * m_Info.m_Size.y;
        float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.y;
        float Advance = ((float)(glyph_pos[i].x_advance - 4) / FontHeight) * m_Info.m_Size.y;
        float BearingX = ((float)((m_Face->FTFace->glyph->metrics.horiBearingX >> 6) - 4)/ FontHeight) * m_Info.m_Size.y;
        float BearingY = ((float)((m_Face->FTFace->glyph->metrics.horiBearingY >> 6) - 4)/ FontHeight) * m_Info.m_Size.y;
        float Baseline = ((float)(((-m_Face->FTFace->size->metrics.descender >> 6) + 8) / FontHeight)) * m_Info.m_Size.y;
        x += m_Kerning;
        Result.push_back({x + BearingX, Baseline + (BearingY - Height), Width, Height});
        x += Advance;
    }

    hb_buffer_destroy(HBBuffer);
    return Result;
}


void fx_Text::Update()
{
    for (auto &x : m_Objects)
    {
        delete x;
    }
    m_Objects.clear();
    m_Objects.resize(0);
    m_Objects.reserve(m_Text.size());

    glm::vec3 Scale = {0, m_Info.m_Size.y, 1};

    std::vector<fx_CharInfo> Layout = GetTextLayout(m_Text);
    for (unsigned int i = 0; i < Layout.size(); i++)
    {
        glm::vec3 FontPos = {Layout[i].X, Layout[i].Y, 0.0f};
        glm::vec3 OffsetPos = { m_Info.m_Anchor.x * Scale.x, m_Info.m_Anchor.y * Scale.y, 0};
        glm::vec3 GlyphPos = glm::toMat3(m_Info.m_Rotation) * (FontPos - OffsetPos);
        fx_UV CharTexturePos;
        if (m_FontAtlas.Coord[m_FontOffset + m_Text[i]].H * m_FontAtlas.Coord[m_FontOffset + m_Text[i]].W == 0)
        {
            CharTexturePos = fx_Atlas::GetUV( m_FontOffset, m_FontAtlas, 5, 5);
        }
        else
        {
            CharTexturePos = fx_Atlas::GetUV( m_FontOffset + m_Text[i], m_FontAtlas, 5, 5);
        }
        if (m_Face->SDF)
        {
            fx_SDF *Character = new fx_SDF(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
            Character->m_GlowThreshold = m_GlowThreshold;
            Character->m_GlowColor = m_GlowColor;
            Character->m_OutlineThreshold = m_OutlineThreshold;
            Character->m_OutlineColor = m_OutlineColor;
            Character->m_Info.m_Rotation = m_Info.m_Rotation;
            Character->Update();
            m_Objects.push_back(Character);
        }
        else
        {
            fx_Sprite *Character = new fx_Sprite(m_Info.m_Position + GlyphPos, glm::vec2(Layout[i].Width ,Layout[i].Height), CharTexturePos, m_Info.m_Color);
            Character->m_Info.m_Rotation = m_Info.m_Rotation;
            Character->Update();
            m_Objects.push_back(Character);
        }
    }
    for (auto x : m_Objects)
    {
        x->Update();
    }
    // GenerateMesh();
}
