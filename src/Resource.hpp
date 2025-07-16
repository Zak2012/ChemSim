#pragma once

#include <vector>
#include <string>
#include <exception>
#include <stdexcept>

#include <glm/glm.hpp>

struct fx_Rect
{
    unsigned int X;
    unsigned int Y;
    unsigned int W;
    unsigned int H;
    fx_Rect static PackRect(std::vector<fx_Rect> &Rectangles);
};

struct fx_UV
{
    float X1;
    float Y1;
    float X2;
    float Y2;
};

struct fx_Image
{
    unsigned int Width;
    unsigned int Height;
    unsigned int Component;
    std::vector<uint8_t> Data;
    // static fx_Image LoadImage(std::string Path);
    static fx_Image LoadPNG(std::vector<uint8_t> Data);
    static fx_Image LoadSVG(std::vector<uint8_t> Data);
    static void FlipImageVert(fx_Image &Image);
    static std::vector<uint8_t> EncodePNG(const fx_Image &Image);
    static fx_Image PadImage(const fx_Image &Image, unsigned int EdgePadding, unsigned int Padding);
};

struct fx_Atlas
{
    fx_Image Image;
    std::vector<fx_Rect> Coord;

    static fx_UV GetUV(unsigned int Index, const fx_Atlas &Atlas, unsigned int Padding = 0, unsigned int EdgePadding = 0);
    static fx_Atlas PackImages(const std::vector<fx_Image> &Images);
    static fx_Atlas Add(const std::vector<fx_Atlas> &Item);
    static std::vector<uint8_t> SaveAtlas(const fx_Atlas &Atlas);
    static fx_Atlas ReadAtlas(const std::vector<uint8_t> &Atlas);
};