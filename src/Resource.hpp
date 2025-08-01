#pragma once

#include <vector>
#include <string>
#include <map>

#include <glm/glm.hpp>

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
    static fx_Image LoadPNG(std::vector<uint8_t> Data);
    static fx_Image LoadSVG(std::vector<uint8_t> Data);
    static void FlipImageVert(fx_Image &Image);
    static std::vector<uint8_t> EncodePNG(const fx_Image &Image);
};

struct fx_AtlasItem
{
    unsigned int X;
    unsigned int Y;
    unsigned int W;
    unsigned int H;
    fx_Image Image;
    fx_UV UV;
};

struct fx_Atlas
{
    fx_Image Image;
    std::map<std::string, fx_AtlasItem> ImagesList;

    static void PackImages(fx_Atlas &Atlas);
    // static fx_Atlas PackImages(const std::map<std::string, fx_AtlasItem> &Images);
    static std::vector<uint8_t> SaveAtlas(const fx_Atlas &Atlas);
    static fx_Atlas ReadAtlas(const std::vector<uint8_t> &Atlas);
};