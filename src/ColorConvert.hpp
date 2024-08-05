#pragma once

#include <vector>

namespace ColorConvert
{
    std::vector<unsigned char> Gray2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> Gray2GrayA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> Gray2RGB(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> Gray2RGBA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> GrayA2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> GrayA2RGB(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> GrayA2RGBA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGB2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGB2GrayA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGB2BGR(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGB2BGRA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGB2RGBA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> BGR2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> BGR2GrayA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGBA2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGBA2GrayA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGBA2RGB(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGBA2BGR(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> RGBA2BGRA(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> BGRA2Gray(const std::vector<unsigned char> &Input);
    std::vector<unsigned char> BGRA2GrayA(const std::vector<unsigned char> &Input);

    enum class ColorClass
    {
        Gray,
        GrayA,
        RGB,
        BGR,
        RGBA,
        BGRA
    };

    typedef std::vector<unsigned char> (*ColorConvertFunc)(const std::vector<unsigned char>&);

    /*
    +──────────+────────────+─────────────+───────────+───────────+────────────+────────────+
    | From/To  | Gray       | GrayA       | RGB       | BGR       | RGBA       | BGRA       |
    +──────────+────────────+─────────────+───────────+───────────+────────────+────────────+
    | Gray     | Gray2Gray  | Gray2GrayA  | Gray2RGB  | Gray2RGB  | Gray2RGBA  | Gray2RGBA  |
    | GrayA    | GrayA2Gray | Gray2Gray   | GrayA2RGB | GrayA2RGB | GrayA2RGBA | GrayA2RGBA |
    | RGB      | RGB2Gray   | RGB2GrayA   | Gray2Gray | RGB2BGR   | RGB2RGBA   | RGB2BGRA   |
    | BGR      | BGR2Gray   | BGR2GrayA   | RGB2BGR   | Gray2Gray | RGB2BGRA   | RGB2RGBA   |
    | RGBA     | RGBA2Gray  | RGBA2GrayA  | RGBA2RGB  | RGBA2BGR  | Gray2Gray  | RGBA2BGRA  |
    | BGRA     | BGRA2Gray  | BGRA2GrayA  | RGBA2BGR  | RGBA2RGB  | RGBA2BGRA  | Gray2Gray  |
    +──────────+────────────+─────────────+───────────+───────────+────────────+────────────+
    */
    constexpr ColorConvertFunc GetColorConvertFunc(const ColorClass From, const ColorClass To) noexcept
    {
        switch (From)
        {
        case ColorClass::Gray:
            switch (To)
            {
            case ColorClass::Gray: return Gray2Gray;
            case ColorClass::GrayA: return Gray2GrayA;
            case ColorClass::RGB: return Gray2RGB;
            case ColorClass::BGR: return Gray2RGB;
            case ColorClass::RGBA: return Gray2RGBA;
            case ColorClass::BGRA: return Gray2RGBA;
            }
        case ColorClass::GrayA:
            switch (To)
            {
            case ColorClass::Gray: return GrayA2Gray;
            case ColorClass::GrayA: return Gray2Gray;
            case ColorClass::RGB: return GrayA2RGB;
            case ColorClass::BGR: return GrayA2RGB;
            case ColorClass::RGBA: return GrayA2RGBA;
            case ColorClass::BGRA: return GrayA2RGBA;
            }
        case ColorClass::RGB:
            switch (To)
            {
            case ColorClass::Gray: return RGB2Gray;
            case ColorClass::GrayA: return RGB2GrayA;
            case ColorClass::RGB: return Gray2Gray;
            case ColorClass::BGR: return RGB2BGR;
            case ColorClass::RGBA: return RGB2RGBA;
            case ColorClass::BGRA: return RGB2BGRA;
            }
        case ColorClass::BGR:
            switch (To)
            {
            case ColorClass::Gray: return BGR2Gray;
            case ColorClass::GrayA: return BGR2GrayA;
            case ColorClass::RGB: return RGB2BGR;
            case ColorClass::BGR: return Gray2Gray;
            case ColorClass::RGBA: return RGB2BGRA;
            case ColorClass::BGRA: return RGB2RGBA;
            }
        case ColorClass::RGBA:
            switch (To)
            {
            case ColorClass::Gray: return RGBA2Gray;
            case ColorClass::GrayA: return RGBA2GrayA;
            case ColorClass::RGB: return RGBA2RGB;
            case ColorClass::BGR: return RGBA2BGR;
            case ColorClass::BGRA: return RGBA2BGRA;
            case ColorClass::RGBA: return Gray2Gray;
            }
        case ColorClass::BGRA:
            switch (To)
            {
            case ColorClass::Gray: return BGRA2Gray;
            case ColorClass::GrayA: return BGRA2GrayA;
            case ColorClass::RGB: return RGBA2BGR;
            case ColorClass::BGR: return RGBA2RGB;
            case ColorClass::BGRA: return Gray2Gray;
            case ColorClass::RGBA: return RGBA2BGRA;
            }
        }
        return nullptr;
    }
}