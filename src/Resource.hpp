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
    static fx_Image LoadImage(std::string Path);
    static void FlipImageVert(fx_Image &Image);
    static void SaveImage(const fx_Image &Image, std::string Filename);
    static std::vector<uint8_t> EncodePNG(const fx_Image &Image);
    static fx_Image PadImage(const fx_Image &Image, unsigned int EdgePadding, unsigned int Padding);
};

struct fx_Atlas
{
    fx_Image Image;
    std::vector<fx_Rect> Coord;

    static fx_UV GetUV(unsigned int Index, const fx_Atlas &Atlas, unsigned int Padding, unsigned int EdgePadding);
    static fx_Atlas PackImages(const std::vector<fx_Image> &Images);
    static fx_Atlas Add(const std::vector<fx_Atlas> &Item);
    static void SaveAtlas(const fx_Atlas &Atlas, std::string Filename);
    static fx_Atlas ReadAtlas(std::string Filename);

};

// class fx_Texture
// {
// private:
//     unsigned int m_TextureID;
//     fx_Image m_Data;
// public:
//     fx_Texture(fx_Image &Data, bool Linear=true);
//     ~fx_Texture();
//     void Update(fx_Image &Data);
//     fx_Image GetData() {return m_Data;}
//     unsigned int GetID() {return m_TextureID;}
//     void Bind();
//     void Unbind();
// };


// class fx_ImageArray
// {
// private:
//     unsigned int m_Width;
//     unsigned int m_Height;
//     unsigned int m_Component;
//     std::vector<uint8_t> m_Data;
// public:
//     fx_ImageArray(unsigned int Width, unsigned int Height, unsigned int Component, const std::vector<uint8_t> &Data);
//     void Add(fx_ImageArray *Data);
//     void Update(unsigned int Index, fx_ImageArray *Data);
//     std::vector<uint8_t> GetData() {return m_Data;}
//     unsigned int GetLayerSize() {return m_Width * m_Height * m_Component;}
//     unsigned int GetDepth() {return m_Data.size() / GetLayerSize();}
//     unsigned int GetSize() {return m_Data.size();}
//     unsigned int GetWidth() {return m_Width;}
//     unsigned int GetHeight() {return m_Height;}
//     unsigned int GetComponent() {return m_Component;}
// };

// class fx_Texture
// {
// private:
//     unsigned int m_TextureID;
//     fx_ImageArray *m_Data;
// public:
//     fx_Texture(fx_ImageArray *Data);
//     ~fx_Texture() {glDeleteTextures(1, &m_TextureID);}
//     void Update(fx_ImageArray *Data);
//     fx_ImageArray* GetData() {return m_Data;}
//     unsigned int GetID() {return m_TextureID;}
//     void Bind() {glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureID);}
//     void Unbind() {glBindTexture(GL_TEXTURE_2D_ARRAY, 0);}
// };

// fx_ImageArray fx_LoadImage(std::string Path);