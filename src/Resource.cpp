#include "Resource.hpp"

#include <vector>
#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <cmath>
#include <fstream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <rectpack2D/finders_interface.h>
#include <tinyxml2/tinyxml2.h>
#include <glm/glm.hpp>

#include "File.hpp"

fx_Rect fx_Rect::PackRect(std::vector<fx_Rect> &Rectangles)
{
    fx_Rect Result;
    constexpr bool allow_flip = false;
	const auto runtime_flipping_mode = rectpack2D::flipping_option::ENABLED;

	using spaces_type = rectpack2D::empty_spaces<allow_flip, rectpack2D::default_empty_spaces>;
	using rect_type = rectpack2D::output_rect_t<spaces_type>;

    auto report_successful = [](rect_type&) {
		return rectpack2D::callback_result::CONTINUE_PACKING;
	};

	auto report_unsuccessful = [](rect_type&) {
		return rectpack2D::callback_result::ABORT_PACKING;
	};

    unsigned int MaxSize = 0;

    for (auto x : Rectangles)
    {
        MaxSize += x.W * x.H;
    }

    MaxSize *= 2;

    int DiscardStep = -5;

    rectpack2D::rect_xywh *RectArray = (rectpack2D::rect_xywh*)Rectangles.data();

    std::vector<rectpack2D::rect_xywh> ResultRectangle(RectArray, RectArray + Rectangles.size());

    const auto result_size = find_best_packing<spaces_type>(
        ResultRectangle,
        make_finder_input(
            std::sqrt(MaxSize),
            DiscardStep,
            report_successful,
            report_unsuccessful,
            runtime_flipping_mode
        )
    );

    Result.X = 0;
    Result.Y = 0;
    Result.W = result_size.w;
    Result.H = result_size.h;

    fx_Rect *ResultArray = (fx_Rect*)ResultRectangle.data();
    Rectangles = std::vector<fx_Rect>(ResultArray, ResultArray + ResultRectangle.size());

    return Result;

}


fx_Image fx_Image::LoadImage(std::string Path)
{
    int ImgWidth, ImgHeight, Component;
    uint8_t* PicData = stbi_load(Path.c_str(), &ImgWidth, &ImgHeight, &Component, 0);

    if (!PicData)
    {
        throw std::runtime_error("Failed to load image");
    }
    
    std::vector<uint8_t> Data(PicData, PicData + (ImgWidth * ImgHeight * Component));
    fx_Image Result;
    
    Result.Width = (unsigned int)ImgWidth;
    Result.Height = (unsigned int)ImgHeight;
    Result.Component = (unsigned int)Component;
    Result.Data = Data;

    stbi_image_free(PicData);

    return Result;
}

void fx_Image::FlipImageVert(fx_Image &Image)
{
   for(unsigned int line = 0; line != Image.Height/2; ++line) {
    std::swap_ranges(
            Image.Data.begin() + Image.Component * Image.Width * line,
            Image.Data.begin() + Image.Component * Image.Width * (line+1),
            Image.Data.begin() + Image.Component * Image.Width * (Image.Height-line-1));
    } 
}

std::vector<uint8_t> fx_Image::EncodePNG(const fx_Image &Image)
{
    int Size = 0;
    unsigned char* Encoded = stbi_write_png_to_mem(Image.Data.data(), Image.Width * Image.Component, Image.Width, Image.Height, Image.Component, &Size);
    return std::vector<uint8_t>(Encoded, Encoded+Size);
}

void fx_Image::SaveImage(const fx_Image &Image, std::string Filename)
{
    stbi_write_png(Filename.c_str(), Image.Width, Image.Height, Image.Component, Image.Data.data(), Image.Width * Image.Component);
}

fx_Image fx_Image::PadImage(const fx_Image &Image, unsigned int EdgePad, unsigned int Pad)
{
    fx_Image Result;

    if (Result.Height * Result.Width == 0)
    {
        return Image;
    }

    Result.Component = Image.Component;

    Result.Height = Image.Height + (Pad * 2) + (EdgePad * 2);
    Result.Width = Image.Width + (Pad * 2) + (EdgePad * 2);

    unsigned int ImageSizeByte = Result.Height * Result.Width * Image.Component;

    unsigned int startX = EdgePad + Pad;
    unsigned int startY = EdgePad + Pad;
    unsigned int endX = startX + Image.Width - 1;
    unsigned int endY = startY + Image.Height - 1;
    unsigned int IndexOffset = (startY * Result.Width) + startX;

    Result.Data.resize(ImageSizeByte);
    std::fill(Result.Data.begin(), Result.Data.end(), 0);

    if (Image.Height * Image.Width * Image.Component == 0)
    {
        return Result;
    }

    for (unsigned int i = 0; i < Image.Height; i++)
    {
        std::copy(
            Image.Data.begin() + (i * Image.Width * Image.Component), 
            Image.Data.begin() + ((i + 1) * Image.Width * Image.Component), 
            Result.Data.begin() + ((IndexOffset + (i * Result.Width)) * Image.Component)
            );
    }

    for (unsigned int i = 0; i < Result.Width * Result.Height; i++)
    {
        unsigned int PosX = i % Result.Width;
        unsigned int PosY = i / Result.Width;

        if (PosX >= startX && PosX <= endX && PosY >= startY && PosY <= endY)
        {
            continue;
        }
        else if (PosX < Pad || PosX >= Result.Width  - Pad || PosY < Pad || PosY >= Result.Height - Pad)
        {
            continue;
        }
        else if (PosX <= startX && PosX >= Pad && PosY <= startY && PosY >= Pad && !(PosX == startX && PosY == startY))
        {
            std::copy(
                Image.Data.begin(),
                Image.Data.begin() + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX >= endX && PosX < Result.Width - Pad && PosY <= startY && PosY >= Pad && !(PosX == endX && PosY == startY))
        {
            std::copy(
                Image.Data.begin() + ((Image.Width - 1) * Image.Component),
                Image.Data.begin() + ((Image.Width - 1) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX <= startX && PosX >= Pad && PosY >= endY && PosY < Result.Height - Pad && !(PosX == startX && PosY == endY))
        {
            std::copy(
                Image.Data.begin() + (((Image.Height * Image.Width) - Image.Width) * Image.Component),
                Image.Data.begin() + (((Image.Height * Image.Width) - Image.Width) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX >= endX && PosX < Result.Width - Pad && PosY >= endY && PosY < Result.Height - Pad && !(PosX == endX && PosY == endY))
        {
            std::copy(
                Image.Data.begin() + ((Image.Height * Image.Width - 1) * Image.Component),
                Image.Data.begin() + ((Image.Height * Image.Width - 1) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX >= Pad && PosX <= endX && PosY < startY && PosY >= Pad)
        {
            std::copy(
                Image.Data.begin() + ((PosX - startX) * Image.Component),
                Image.Data.begin() + ((PosX - startX) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX < startX && PosX >= Pad && PosY >= startY && PosY <= endY)
        {
            std::copy(
                Image.Data.begin() + (((PosY - startY) * Image.Width) * Image.Component),
                Image.Data.begin() + (((PosY - startY) * Image.Width) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX > endX && PosX < Result.Width - Pad && PosY > startY && PosY < endY)
        {
            std::copy(
                Image.Data.begin() + ((((PosY - startY + 1) * Image.Width ) - 1) * Image.Component),
                Image.Data.begin() + ((((PosY - startY + 1) * Image.Width ) - 1) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
        else if (PosX >= startX && PosX <= endX && PosY > endY && PosY < Result.Height - Pad)
        {
            std::copy(
                Image.Data.begin() + (((Image.Height * Image.Width) - Image.Width + (PosX - startX + 1) - 1) * Image.Component),
                Image.Data.begin() + (((Image.Height * Image.Width) - Image.Width + (PosX - startX + 1) - 1) * Image.Component) + Image.Component,
                Result.Data.begin() + (i * Image.Component)
            );
        }
    }
    return Result;
}


fx_UV fx_Atlas::GetUV(unsigned int Index, const fx_Atlas &Atlas, unsigned int Padding, unsigned int EdgePadding)
{
    fx_UV UV;
    fx_Rect Rect = Atlas.Coord[Index];
    UV.X1 = (float)(Rect.X + (Padding + EdgePadding)) / (float)Atlas.Image.Width;
    UV.Y1 = (float)(Rect.Y + (Padding + EdgePadding)) / (float)Atlas.Image.Height;
    UV.X2 = (float)(Rect.X - (Padding + EdgePadding) + Rect.W) / (float)Atlas.Image.Width;
    UV.Y2 = (float)(Rect.Y - (Padding + EdgePadding) + Rect.H) / (float)Atlas.Image.Height;
    return UV;
}


fx_Atlas fx_Atlas::PackImages(const std::vector<fx_Image> &Images)
{
    fx_Atlas Result;
    Result.Coord = std::vector<fx_Rect>();
    Result.Coord.reserve(Images.size());

    unsigned int Component = Images[0].Component;
    for (const auto &x : Images)
    {
        Result.Coord.push_back(fx_Rect({0,0,x.Width,x.Height}));
        if (x.Component != Component)
        {
            throw std::runtime_error("Unequal Component");
        }

    }

    fx_Rect AtlasRect = fx_Rect::PackRect(Result.Coord);
    Result.Image.Component = Component;
    Result.Image.Height = AtlasRect.H;
    Result.Image.Width = AtlasRect.W;
    Result.Image.Data = std::vector<unsigned char>();
    Result.Image.Data.resize(Result.Image.Width * Result.Image.Height * Result.Image.Component);
    std::fill(Result.Image.Data.begin(), Result.Image.Data.end(), 0);

    // for (unsigned int i = 0; i < Images.size(); i++)
    // {
    //     Images[i].ImagePos = Rectangles[i];
    // }
    
    for (unsigned int i = 0; i < Images.size(); i++)
    {
        unsigned int IndexOffset = (Result.Coord[i].Y * Result.Image.Width) + Result.Coord[i].X;
        for (unsigned int j = 0; j < Images[i].Height; j++)
        {
            // FinalImage[(startY * ImageHeight) + startX]
            std::copy(
                Images[i].Data.begin() + (j * Images[i].Width * Images[i].Component), 
                Images[i].Data.begin() + ((j + 1) * Images[i].Width * Images[i].Component), 
                Result.Image.Data.begin() + ((IndexOffset + (j * Result.Image.Width)) * Images[i].Component)
                );
        }
    }
    return Result;
}

fx_Atlas fx_Atlas::Add(const std::vector<fx_Atlas> &Item)
{
    fx_Atlas Result;
    std::vector<fx_Image> Images;
    Images.reserve(Item.size());

    unsigned int Component = Item[0].Image.Component;
    for (auto x: Item)
    {
        if (x.Image.Component != Component)
        {
            throw std::runtime_error("Unequal Component");
        }
        Images.push_back(x.Image);
    }

    Result = PackImages(Images);
    std::vector<fx_Rect> TempRect = Result.Coord;

    unsigned int ItemSize = 0;
    for (auto x : Item)
    {
        ItemSize += x.Coord.size();
    }
    Result.Coord = std::vector<fx_Rect>();
    Result.Coord.reserve(ItemSize);

    for (unsigned int i = 0; i < Item.size(); i++)
    {
        for (auto x : Item[i].Coord)
        {
            fx_Rect Rect;
            Rect.X = x.X + TempRect[i].X;
            Rect.Y = x.Y + TempRect[i].Y;
            Rect.W = x.W;
            Rect.H = x.H;
            Result.Coord.push_back(Rect);
        }
    }

    return Result;
}

void fx_Atlas::SaveAtlas(const fx_Atlas &Atlas, std::string Filename)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* list = doc.NewElement("list");
    std::vector<uint8_t> PngBuffer = fx_Image::EncodePNG(Atlas.Image);
    for (auto x: Atlas.Coord)
    {
        tinyxml2::XMLElement* Rect = doc.NewElement("rect");
        Rect->SetAttribute("X",x.X);
        Rect->SetAttribute("Y",x.Y);
        Rect->SetAttribute("W",x.W);
        Rect->SetAttribute("H",x.H);
        list->InsertEndChild(Rect);
    }
    doc.InsertEndChild(list);

    tinyxml2::XMLPrinter printer;
    doc.Print( &printer );
    std::vector<uint8_t> DataBuffer((uint8_t*)printer.CStr(), (uint8_t*)(printer.CStr() + printer.CStrSize()));
    // std::vector<uint8_t> Buffer((uint8_t*)Atlas.Coord.data(), (uint8_t*)(Atlas.Coord.data() + Atlas.Coord.size()));

    // ApplyEndian(Buffer);

    PngBuffer.insert(PngBuffer.end(), DataBuffer.begin(), DataBuffer.end());

    fx_WriteBinaryFile(Filename, PngBuffer);
}

fx_Atlas fx_Atlas::ReadAtlas(std::string Filename)
{
    fx_Atlas Result;
    Result.Image = fx_Image::LoadImage(Filename);

    std::vector<uint8_t> PNGBuffer = fx_ReadBinaryFile(Filename);
    std::vector<uint8_t> PngEndPattern = {0xAE, 0x42, 0x60, 0x82};
    std::string Buffer = "";
    // Buffer.resize(0);
    auto it = std::search(std::begin(PNGBuffer), std::end(PNGBuffer), std::begin(PngEndPattern), std::end(PngEndPattern));
    if (it != PNGBuffer.end())
    {
        int i = it - PNGBuffer.begin();
        Buffer.insert(Buffer.end(), PNGBuffer.begin() + (i+PngEndPattern.size()), PNGBuffer.end());
    }
    else
    {
        throw std::runtime_error("Resource.cpp: Non Valid Atlas File");
    }

    std::vector<fx_Rect> Rects;

    tinyxml2::XMLDocument doc;
    // doc2.Parse()
    tinyxml2::XMLError eResult = doc.Parse(Buffer.c_str());
    if (eResult != 0)
    {
        throw std::runtime_error("Non Valid Atlas XML");
    }
    tinyxml2::XMLElement* list = doc.FirstChildElement("list");
    for(tinyxml2::XMLElement* e = list->FirstChildElement("rect"); e != NULL; e = e->NextSiblingElement("rect"))
    {
        fx_Rect ARect;
        ARect.X = std::stoi(e->Attribute("X"));
        ARect.Y = std::stoi(e->Attribute("Y"));
        ARect.W = std::stoi(e->Attribute("W"));
        ARect.H = std::stoi(e->Attribute("H"));
        Rects.push_back(ARect);

    }

    // ApplyEndian(Buffer);

    Result.Coord = Rects;

    return Result;



    // Coord_t Cood;
    // Cood.Coodrinates = (const fx_Rect *)InputData.data;
    // Cood.Num = InputData.size/sizeof(fx_Rect);

    // return std::vector<fx_Rect>(Cood.Coodrinates, Cood.Coodrinates + Cood.Num);
    
}

// fx_ImageArray fx_LoadImage(std::string Path)
// {
//     int ImgWidth, ImgHeight, Component;
//     uint8_t* PicData = stbi_load(Path.c_str(), &ImgWidth, &ImgHeight, &Component, 4);

//     if (!PicData)
//     {
//         throw std::runtime_error("Failed to load image");
//     }
    
//     stbi_set_flip_vertically_on_load(true);  

//     std::vector<uint8_t> Data(PicData, PicData + (ImgWidth * ImgHeight * Component));
//     fx_ImageArray Result((unsigned int)ImgWidth, (unsigned int)ImgHeight, (unsigned int)Component, Data);

//     stbi_image_free(PicData);

//     return Result;
// }

// fx_ImageArray::fx_ImageArray(unsigned int Width, unsigned int Height, unsigned int Component, const std::vector<uint8_t> &Data)
// {
//     m_Width = Width;
//     m_Height = Height;
//     m_Component = Component;
//     m_Data = Data;

//     if (m_Data.size() < GetLayerSize())
//     {
//         m_Data.resize(GetLayerSize());
//     }

// }

// void fx_ImageArray::Add(fx_ImageArray *Data)
// {
//     if (m_Width != Data->GetWidth() || m_Height != Data->GetHeight() || m_Component != Data->GetComponent())
//     {
//         throw std::runtime_error("ImageArray doesn't have the same dimension");
//     }
//     auto DataArray = Data->GetData();
//     m_Data.insert(m_Data.end(), DataArray.begin(), DataArray.end());
    
// }

// void fx_ImageArray::Update(unsigned int Index, fx_ImageArray *Data)
// {
//     if (m_Width != Data->GetWidth() || m_Height != Data->GetHeight() || m_Component != Data->GetComponent())
//     {
//         throw std::runtime_error("ImageArray doesn't have the same dimension");
//     }
//     auto DataArray = Data->GetData();
//     std::copy(
//         DataArray.begin(), 
//         DataArray.end(), 
//         m_Data.begin() + (Index * GetLayerSize())
//     );
// }


// fx_Texture::fx_Texture(fx_ImageArray *Data)
// {
//     m_Data = Data;

//     GLenum Format;
//     GLenum InternalFormat;
//     if (m_Data->GetComponent() == 4)
//     {
//         Format = GL_RGBA8;
//         InternalFormat = GL_RGBA;
//     }
//     else
//     {
//         Format = GL_RGB8;
//         InternalFormat = GL_RGB;
//     }

//     glGenTextures(1,&m_TextureID);
//     Bind();
//     glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, Format, m_Data->GetWidth(), m_Data->GetHeight(), m_Data->GetDepth());
//     glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 
//     m_Data->GetWidth(), m_Data->GetHeight(), m_Data->GetDepth(), 
//     InternalFormat, GL_UNSIGNED_BYTE, m_Data->GetData().data());

//     glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
//     Unbind();
// }

// void fx_Texture::Update(fx_ImageArray *Data)
// {
//     GLenum Format;
//     GLenum InternalFormat;
//     if (m_Data->GetComponent() == 4)
//     {
//         Format = GL_RGBA8;
//         InternalFormat = GL_RGBA;
//     }
//     else
//     {
//         Format = GL_RGB8;
//         InternalFormat = GL_RGB;
//     }

//     if (m_Data->GetWidth() != Data->GetWidth()     || m_Data->GetHeight() != Data->GetHeight() || 
//     m_Data->GetComponent() != Data->GetComponent() || m_Data->GetDepth()  != Data->GetDepth())
//     {
//         glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, Format, Data->GetWidth(), Data->GetHeight(), Data->GetDepth());
//     }

//     m_Data = Data;

//     glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 
//     m_Data->GetWidth(), m_Data->GetHeight(), m_Data->GetDepth(), 
//     InternalFormat, GL_UNSIGNED_BYTE, m_Data->GetData().data());


// }