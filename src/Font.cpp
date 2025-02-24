#include "Font.hpp"

#include <string>
#include <vector>
#include <exception>
#include <algorithm>

// #include <hb/hb.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Resource.hpp"

#define FONT_SIZE_PIXEL 64

static FT_Library FTRuntime = NULL;
static uint32_t FontCount = 0;

// struct fx_Font_Library_s
// {
//     FT_Library FTLibrary;
// };

// fx_Font_Library fx_Load_Lib()
// {
//     fx_Font_Library Lib = new fx_Font_Library_s;
//     if (FT_Init_FreeType(&Lib->FTLibrary))
//     {
//         throw std::runtime_error("Font.cpp: Failed to load FT_LIbrary");
//     }
//     return Lib;
// }

// void fx_Delete_Lib(fx_Font_Library Lib)
// {
//     if (FT_Done_FreeType(Lib->FTLibrary))
//     {
//         throw std::runtime_error("Font.cpp: Failed to delete FT_LIbrary");
//     }
// }

// struct fx_Face_s
// {
//     FT_Face FTFace;
//     // hb_blob_t *HBBlob;
//     // hb_face_t *HBFace;
//     // hb_font_t *HBFont;
//     bool SDF = true;
//     int Size = 64;
// };

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
    FT_Set_Pixel_Sizes((FT_Face)m_FontFace, 0, 64);  

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


// fx_Face fx_Load_Face(fx_Font_Library Lib, std::string FileName)
// {
//     fx_Face Face = new fx_Face_s;
//     int a = FT_New_Face(Lib->FTLibrary, FileName.c_str(), 0, &Face->FTFace);
//     if (a)
//     {
//         throw std::runtime_error("Font.cpp: Failed to load FT_Face");
//     }

//     // Face->HBBlob = hb_blob_create_from_file_or_fail(FileName.c_str());

//     // if (Face->HBBlob == NULL)
//     // {
//     //     throw std::runtime_error("Font.cpp: Failed to load hb_blob_t");
//     // }

//     FT_Set_Pixel_Sizes(Face->FTFace, 0, Face->Size);  

//     // Face->HBFace = hb_face_create(Face->HBBlob, 0);
//     // Face->HBFont = hb_font_create(Face->HBFace);
//     // hb_font_set_scale(Face->HBFont, Face->Size, Face->Size);
//     return Face;
// }

// int fx_Get_Size(fx_Face Face)
// {
//     return Face->Size;
// }
// void fx_Set_Size(fx_Face Face, int Size)
// {
//     Face->Size = Size;
//     FT_Set_Pixel_Sizes(Face->FTFace, 0, Face->Size);  

//     // Face->HBFace = hb_face_create(Face->HBBlob, 0);
//     // Face->HBFont = hb_font_create(Face->HBFace);
//     // hb_font_set_scale(Face->HBFont, Face->Size, Face->Size);
// }

// bool fx_Get_SDF(fx_Face Face)
// {
//     return Face->SDF;
// }
// void fx_Set_SDF(fx_Face Face, bool SDF)
// {
//     Face->SDF = SDF;
// }

// void fx_Delete_Face(fx_Face Face)
// {
//     FT_Done_Face(Face->FTFace);

//     // hb_font_destroy(Face->HBFont);
//     // hb_face_destroy(Face->HBFace);
//     // hb_blob_destroy(Face->HBBlob);
//     delete Face;
// }

// fx_Atlas fx_GenerateAtlas(fx_Face Face, bool IsWideChar)
// {
//     unsigned int MaxGlyph = IsWideChar?USHRT_MAX:CHAR_MAX;
//     fx_Image BlankImage;
//     BlankImage.Width = 0;
//     BlankImage.Height = 0;
//     BlankImage.Component = 1;
//     std::vector<fx_Image> Characters;
//     Characters.resize(MaxGlyph);
//     std::fill(Characters.begin(), Characters.end(), BlankImage);
//     {
//         fx_Image Glyph;
//         if (FT_Load_Char(Face->FTFace, 0, FT_LOAD_DEFAULT))
//         {
//             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//         }
//         if (FT_Render_Glyph(Face->FTFace->glyph, Face->SDF?FT_RENDER_MODE_SDF:FT_RENDER_MODE_NORMAL))
//         {
//             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//         }

//         Glyph.Component = 1;
//         Glyph.Width = Face->FTFace->glyph->bitmap.width;
//         Glyph.Height = Face->FTFace->glyph->bitmap.rows;
//         Glyph.Data = std::vector<unsigned char>(Face->FTFace->glyph->bitmap.buffer, Face->FTFace->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
//         fx_Image::FlipImageVert(Glyph);
//         Characters[0] = fx_Image::PadImage(Glyph, 5, 5);
//     }
//     unsigned int GlyphIndex = 0;
//     unsigned int Charcode = FT_Get_First_Char(Face->FTFace, &GlyphIndex);

//     while ( GlyphIndex != 0 )
//     {
//         fx_Image Glyph;
//         if (FT_Load_Char(Face->FTFace, Charcode, FT_LOAD_DEFAULT))
//         {
//             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//             continue;
//         }
//         if (FT_Render_Glyph(Face->FTFace->glyph, Face->SDF?FT_RENDER_MODE_SDF:FT_RENDER_MODE_NORMAL))
//         {
//             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//             continue;
//         }

//         Glyph.Component = 1;
//         Glyph.Width = Face->FTFace->glyph->bitmap.width;
//         Glyph.Height = Face->FTFace->glyph->bitmap.rows;
//         Glyph.Data = std::vector<unsigned char>(Face->FTFace->glyph->bitmap.buffer, Face->FTFace->glyph->bitmap.buffer + (Glyph.Component * Glyph.Height * Glyph.Width));
//         Characters[Charcode] = fx_Image::PadImage(Glyph, 5, 5);
//         Charcode = FT_Get_Next_Char( Face->FTFace, Charcode, &GlyphIndex );
//         if (Charcode > MaxGlyph)
//         {
//             break;
//         }
//     }
//     fx_Atlas Result = fx_Atlas::PackImages(Characters);
//     return Result;
// }

// void fx_Load_Font(fx_Atlas *InAtlas, fx_Face InFace, std::string Filename, unsigned int Index, bool Wide)
// {
//     if (std::filesystem::exists(Filename))
//     {
//         *InAtlas = fx_Atlas::ReadAtlas(Filename);
//     }
//     else
//     {
//         *InAtlas = fx_GenerateAtlas(InFace, Wide);
//         fx_Atlas::SaveAtlas(*InAtlas, Filename);
//     }
// }

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

fx_TextBox::fx_TextBox(glm::vec3 Pos, float LineHeight, fx_Font *Font, std::string Text, glm::vec4 Colour, glm::vec4 Background)
{
    SetPosition(Pos);
    SetColour(Colour);
    SetText(Text);
    SetFont(Font);
    SetLineHeight(LineHeight);
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;
}

void fx_TextBox::Update()
{   
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    if (m_FlagUpdateObject)
    {
        for (auto &x : m_Objects)
        {
            delete x;
        }
        m_Objects.clear();
        m_Objects.resize(0);
        m_Objects.reserve(m_Text.size());
    }

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
            CharTexturePos = fx_Atlas::GetUV( 0, Atlas, 2);
        }
        else
        {
            CharTexturePos = fx_Atlas::GetUV( m_Text[i], Atlas, 2);
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

inline float FtFloatToFloat(int32_t input)
{
    return input / 64.0f;
}

std::vector<glm::vec4> fx_TextBox::GetTextLayout(std::string Text)
{
    std::vector<glm::vec4> Result;
    Result.resize(Text.size()+1);
    glm::vec3 Scale = {0, m_LineHeight, 1};
    float x = 0;

    // hb_buffer_t *HBBuffer;
    // HBBuffer = hb_buffer_create();
    // hb_buffer_add_utf8(HBBuffer, Text.c_str(), -1, 0, -1);

    // hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);

    // hb_shape(m_Face->HBFont, HBBuffer, NULL, 0);

    unsigned int glyph_count;
    // hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(HBBuffer, &glyph_count);
    // hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(HBBuffer, &glyph_count);

    float Scalingfactor = m_LineHeight / (FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->size->metrics.height));

    // for(unsigned int i = 1; i < Result.size(); i ++)
    // {
    //     float Advance = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.vertAdvance) * Scalingfactor;
    //     Scale.x += m_Kerning;
    //     if (i + 1 == Text.size())
    //     {
    //         FT_Load_Char(((FT_Face)m_Font->m_FontFace), Text[i], FT_LOAD_DEFAULT);
    //         float Width = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.width) * Scalingfactor;
    //         Scale.x += Width;
    //     }
    //     else
    //     {
    //     Scale.x += Advance;
    //     }
    // }
    // Scale -= (8 /FontHeight) * m_LineHeight;

    uint32_t previous    = 0;


    for(unsigned int i = 0; i < Text.size(); i ++)
    {
        uint32_t glyph_index = FT_Get_Char_Index( (FT_Face)m_Font->m_FontFace, Text[i] );
        FT_Error a = FT_Load_Glyph((FT_Face)m_Font->m_FontFace, glyph_index, FT_LOAD_DEFAULT);
        // FT_Error a = FT_Load_Char((FT_Face)m_Font->m_FontFace, Text[i], FT_LOAD_DEFAULT);
        float Height = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.height);
        float Width = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.width);
        float Advance = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiAdvance);
        float BearingX = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiBearingX);
        float BearingY = FtFloatToFloat(((FT_Face)m_Font->m_FontFace)->glyph->metrics.horiBearingY);
        x += m_Kerning;

        FT_Vector  delta;



        FT_Get_Kerning( (FT_Face)m_Font->m_FontFace, previous, glyph_index, FT_KERNING_DEFAULT, &delta );

        x += FtFloatToFloat(delta.x);


        // x -= 10;
        Result[i+1] = {x * Scalingfactor , (BearingY - Height) * Scalingfactor , 0, (Height)* Scalingfactor};
        
        x += Advance;
        previous = glyph_index;
    }

    Result[0] = {0,0, x * Scalingfactor, m_LineHeight};

    // hb_buffer_destroy(HBBuffer);
    return Result;
}

// fx_Text::fx_Text(glm::vec3 Pos, glm::vec2 Size, fx_Face Face, std::string Text, unsigned int FontOffset, const fx_Atlas &Atlas, glm::vec4 Color)
// {
//     m_Info.m_Size = glm::vec3(0.0f, Size.y, 1.0f);
//     m_Info.m_Position = Pos;
//     m_Info.m_Color = Color;
//     m_Objects = {};
//     m_Text = Text;
//     m_FontAtlas = Atlas;
//     m_FontOffset = FontOffset;
//     m_Face = Face;
//     m_Drawable = true;
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

// std::vector<fx_CharInfo> fx_Text::GetTextLayout(std::string Text)
// {
//     std::vector<fx_CharInfo> Result;
//     Result.reserve(Text.size());
//     glm::vec3 Scale = {0, m_Info.m_Size.y, 1};
//     float x = 0;

//     // hb_buffer_t *HBBuffer;
//     // HBBuffer = hb_buffer_create();
//     // hb_buffer_add_utf8(HBBuffer, Text.c_str(), -1, 0, -1);

//     // hb_buffer_set_direction(HBBuffer, HB_DIRECTION_LTR);

//     // hb_shape(m_Face->HBFont, HBBuffer, NULL, 0);

//     unsigned int glyph_count;
//     // hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(HBBuffer, &glyph_count);
//     // hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(HBBuffer, &glyph_count);

//     float FontHeight = (float)((m_Face->FTFace->size->metrics.ascender - m_Face->FTFace->size->metrics.descender) >> 6) + 8.0f;


//     for(unsigned int i = 0; i < Text.size(); i ++)
//     {
//         float Advance = ((float)(m_Face->FTFace->glyph->metrics.vertAdvance - 4) / FontHeight) * m_Info.m_Size.x;
//         Scale.x += m_Kerning;
//         if (i + 1 == Text.size())
//         {
//             FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
//             float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.x;
//             Scale.x += Width;
//         }
//         else
//         {
//         Scale.x += Advance;
//         }
//     }
//     Scale -= (8 /FontHeight) * m_Info.m_Size.y;
//     for(unsigned int i = 0; i < Text.size(); i ++)
//     {
//         FT_Load_Char(m_Face->FTFace, Text[i], FT_LOAD_DEFAULT);
//         float Height = ((float)((m_Face->FTFace->glyph->metrics.height >> 6) + 8)/ FontHeight) * m_Info.m_Size.y;
//         float Width = ((float)((m_Face->FTFace->glyph->metrics.width >> 6) + 8) / FontHeight) * m_Info.m_Size.y;
//         float Advance = ((float)(m_Face->FTFace->glyph->metrics.vertAdvance - 4) / FontHeight) * m_Info.m_Size.y;
//         float BearingX = ((float)((m_Face->FTFace->glyph->metrics.horiBearingX >> 6) - 4)/ FontHeight) * m_Info.m_Size.y;
//         float BearingY = ((float)((m_Face->FTFace->glyph->metrics.horiBearingY >> 6) - 4)/ FontHeight) * m_Info.m_Size.y;
//         float Baseline = ((float)(((-m_Face->FTFace->size->metrics.descender >> 6) + 8) / FontHeight)) * m_Info.m_Size.y;
//         x += m_Kerning;
//         Result.push_back({x + BearingX, Baseline + (BearingY - Height), Width, Height});
//         x += Advance;
//     }

//     // hb_buffer_destroy(HBBuffer);
//     return Result;
// }


// void fx_Text::Update()
// {
//     for (auto &x : m_Objects)
//     {
//         delete x;
//     }
//     m_Objects.clear();
//     m_Objects.resize(0);
//     m_Objects.reserve(m_Text.size());

//     glm::vec3 Scale = {0, m_Info.m_Size.y, 1};

//     std::vector<fx_CharInfo> Layout = GetTextLayout(m_Text);
//     for (unsigned int i = 0; i < Layout.size(); i++)
//     {
//         glm::vec3 FontPos = {Layout[i].X, Layout[i].Y, 0.0f};
//         glm::vec3 OffsetPos = { m_Info.m_Anchor.x * Scale.x, m_Info.m_Anchor.y * Scale.y, 0};
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
//     // GenerateMesh();
// }
