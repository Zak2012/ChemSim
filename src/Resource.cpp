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
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

// #include <msdfgen/msdfgen.h>
// #include <msdfgen/msdfgen-ext.h>

// #define XML_H_IMPLEMENTATION // Must be defined before including xml.h in ONE source file
// #include "xml.h"

#include <rectpack2D/finders_interface.h>
#include <tinyxml2.h>
#include <glm/glm.hpp>

// #include "File.hpp"

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


// fx_Image fx_Image::LoadImage(std::string Path)
// {
//     int ImgWidth, ImgHeight, Component;
//     uint8_t* PicData = stbi_load(Path.c_str(), &ImgWidth, &ImgHeight, &Component, 0);

//     if (!PicData)
//     {
//         std::cout << "Failed to load image\n";
//     }
    
//     fx_Image Result;
    
//     Result.Width = (unsigned int)ImgWidth;
//     Result.Height = (unsigned int)ImgHeight;
//     Result.Component = (unsigned int)Component;
//     Result.Data = std::vector<uint8_t> (PicData, PicData + (ImgWidth * ImgHeight * Component));;

//     stbi_image_free(PicData);

//     return Result;
// }

fx_Image fx_Image::LoadPNG(std::vector<uint8_t> Data)
{
    int ImgWidth, ImgHeight, Component;
    uint8_t* PicData = stbi_load_from_memory(Data.data(), Data.size(), &ImgWidth, &ImgHeight, &Component, 0);

    if (!PicData)
    {
        std::cout << "Failed to load image\n";
    }
    
    fx_Image Result;
    
    Result.Width = (unsigned int)ImgWidth;
    Result.Height = (unsigned int)ImgHeight;
    Result.Component = (unsigned int)Component;
    Result.Data = std::vector<uint8_t> (PicData, PicData + (ImgWidth * ImgHeight * Component));

    stbi_image_free(PicData);

    return Result;
}

fx_Image fx_Image::LoadSVG(std::vector<uint8_t> Data)
{
    // msdfgen::loadSvgShape()
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

// void fx_Image::SaveImage(const fx_Image &Image, std::string Filename)
// {
//     stbi_write_png(Filename.c_str(), Image.Width, Image.Height, Image.Component, Image.Data.data(), Image.Width * Image.Component);
// }

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
    fx_Rect Rect = Atlas.CoordList[Index];
    UV.X1 = (float)(Rect.X + (Padding + EdgePadding)) / (float)Atlas.Image.Width;
    UV.Y1 = (float)(Rect.Y + (Padding + EdgePadding)) / (float)Atlas.Image.Height;
    UV.X2 = (float)(Rect.X - (Padding + EdgePadding) + Rect.W) / (float)Atlas.Image.Width;
    UV.Y2 = (float)(Rect.Y - (Padding + EdgePadding) + Rect.H) / (float)Atlas.Image.Height;
    return UV;
}


fx_Atlas fx_Atlas::PackImages(const std::vector<fx_Image> &Images)
{
    fx_Atlas Result;
    Result.ImagesList = Images;
    Result.CoordList = std::vector<fx_Rect>();
    Result.CoordList.reserve(Images.size());

    unsigned int Component = Images[0].Component;
    for (const auto &x : Images)
    {
        Result.CoordList.push_back(fx_Rect({0,0,x.Width,x.Height}));
        if (x.Component != Component)
        {
            throw std::runtime_error("Unequal Component");
        }

    }

    fx_Rect AtlasRect = fx_Rect::PackRect(Result.CoordList);
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
        unsigned int IndexOffset = (Result.CoordList[i].Y * Result.Image.Width) + Result.CoordList[i].X;
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

// fx_Atlas fx_Atlas::Add(const std::vector<fx_Atlas> &Item)
// {
//     fx_Atlas Result;
//     std::vector<fx_Image> Images;
//     Images.reserve(Item.size());

//     unsigned int Component = Item[0].Image.Component;
//     for (auto x: Item)
//     {
//         if (x.Image.Component != Component)
//         {
//             throw std::runtime_error("Unequal Component");
//         }
//         Images.push_back(x.Image);
//     }

//     Result = PackImages(Images);
//     std::vector<fx_Rect> TempRect = Result.CoordList;

//     unsigned int ItemSize = 0;
//     for (auto x : Item)
//     {
//         ItemSize += x.CoordList.size();
//     }
//     Result.CoordList = std::vector<fx_Rect>();
//     Result.CoordList.reserve(ItemSize);

//     for (unsigned int i = 0; i < Item.size(); i++)
//     {
//         for (auto x : Item[i].CoordList)
//         {
//             fx_Rect Rect;
//             Rect.X = x.X + TempRect[i].X;
//             Rect.Y = x.Y + TempRect[i].Y;
//             Rect.W = x.W;
//             Rect.H = x.H;
//             Result.CoordList.push_back(Rect);
//         }
//     }

//     return Result;
// }

std::vector<uint8_t> fx_Atlas::SaveAtlas(const fx_Atlas &Atlas)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* list = doc.NewElement("list");
    std::vector<uint8_t> PngBuffer = fx_Image::EncodePNG(Atlas.Image);

    for (auto x: Atlas.CoordList)
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

    PngBuffer.insert(PngBuffer.end(), DataBuffer.begin(), DataBuffer.end());

    return PngBuffer;
}

fx_Atlas fx_Atlas::ReadAtlas(const std::vector<uint8_t> &Atlas)
{
    fx_Atlas Result;
    Result.Image = fx_Image::LoadPNG(Atlas);

    std::vector<uint8_t> PngEndPattern = {0xAE, 0x42, 0x60, 0x82};
    
    std::string Buffer = "";
    // Buffer.resize(0);
    auto it = std::search(std::begin(Atlas), std::end(Atlas), std::begin(PngEndPattern), std::end(PngEndPattern));
    if (it != Atlas.end())
    {
        int i = it - Atlas.begin();
        Buffer.insert(Buffer.end(), Atlas.begin() + (i+PngEndPattern.size()), Atlas.end());
    }
    else
    {
        std::cout << "Resource.cpp: Non Valid Atlas File\n";
    }

    std::vector<fx_Rect> Rects;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(Buffer.c_str());
    if (eResult != 0)
    {
        std::cout << "Non Valid Atlas XML\n";
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

    Result.CoordList = Rects;

    return Result;
}