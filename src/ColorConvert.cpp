#include "ColorConvert.hpp"

#include <vector>

namespace ColorConvert
{
    std::vector<unsigned char> Gray2Gray(const std::vector<unsigned char> &Input)
    {
        return Input;
    }

    std::vector<unsigned char> Gray2GrayA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()*2);

        for (unsigned int i = 0; i < Input.size(); i++)
        {
            Output.push_back(Input.at(i));
            Output.push_back(255);
        }
        return Output;
    }

    // Combatible for Gray2BGR
    std::vector<unsigned char> Gray2RGB(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()*3);

        for (unsigned int i = 0; i < Input.size(); i++)
        {
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
        }
        return Output;
    }

    // Combatible for Gray2BGRA
    std::vector<unsigned char> Gray2RGBA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()*4);

        for (unsigned int i = 0; i < Input.size(); i++)
        {
            Output.push_back(255);
            Output.push_back(255);
            Output.push_back(255);
            Output.push_back(Input.at(i));
        }
        return Output;
    }

    std::vector<unsigned char> GrayA2Gray(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/2);

        for (unsigned int i = 0; i < Input.size(); i += 2 )
        {
            Output.push_back(Input.at(i));
        }

        return Output;
    }

    // Compatible for GrayA2BGR
    std::vector<unsigned char> GrayA2RGB(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/2*3);

        for (unsigned int i = 0; i < Input.size(); i += 2 )
        {
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
        }

        return Output;
    }

    // Compatible for GrayA2BGRA
    std::vector<unsigned char> GrayA2RGBA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()*2);

        for (unsigned int i = 0; i < Input.size(); i += 2 )
        {
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i+1));
        }

        return Output;
    }

    std::vector<unsigned char> RGB2Gray(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            float Red = (float)Input.at(i) * 0.299f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Green = (float)Input.at(i+2) * 0.114f;
            Output.push_back((unsigned int)(Red + Blue + Green));
        }

        return Output;
    }

    std::vector<unsigned char> RGB2GrayA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3*2);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            float Red = (float)Input.at(i) * 0.299f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Green = (float)Input.at(i+2) * 0.114f;
            Output.push_back((unsigned int)(Red + Blue + Green));
            Output.push_back(255);
        }

        return Output;
    }

    // Compatible for BGR2RGB
    std::vector<unsigned char> RGB2BGR(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size());

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            Output.push_back(Input.at(i+2));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i));
        }

        return Output;
    }

    //Compatible for BGR2RGBA
    std::vector<unsigned char> RGB2BGRA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3*4);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            Output.push_back(Input.at(i+2));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i));
            Output.push_back(255);
        }

        return Output;
    }

    // Compatible for BGR2BGRA
    std::vector<unsigned char> RGB2RGBA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3*4);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i+2));
            Output.push_back(255);
        }

        return Output;
    }

    std::vector<unsigned char> BGR2Gray(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            float Green = (float)Input.at(i) * 0.114f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Red = (float)Input.at(i+2) * 0.299f;
            Output.push_back((unsigned int)(Green + Blue + Red));
        }

        return Output;
    }

    std::vector<unsigned char> BGR2GrayA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/3*2);

        for (unsigned int i = 0; i < Input.size(); i += 3 )
        {
            float Green = (float)Input.at(i) * 0.114f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Red = (float)Input.at(i+2) * 0.299f;
            Output.push_back((unsigned int)(Green + Blue + Red));
            Output.push_back(255);
        }

        return Output;
    }

    std::vector<unsigned char> RGBA2Gray(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/4);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            float Red = (float)Input.at(i) * 0.299f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Green = (float)Input.at(i+2) * 0.114f;
            Output.push_back((unsigned int)(Red + Blue + Green));
        }

        return Output;
    }

    std::vector<unsigned char> RGBA2GrayA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/2);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            float Red = (float)Input.at(i) * 0.299f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Green = (float)Input.at(i+2) * 0.114f;
            Output.push_back((unsigned int)(Red + Blue + Green));
            Output.push_back(Input.at(i+3));
        }

        return Output;
    }
    // Compatible for BGRA2BGR
    std::vector<unsigned char> RGBA2RGB(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/4*3);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i+2));
        }

        return Output;
    }

    // Compatible for BGRA2RGB
    std::vector<unsigned char> RGBA2BGR(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/4*3);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            Output.push_back(Input.at(i+2));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i));
        }

        return Output;
    }

    // Compatible for BGRA2RGBA
    std::vector<unsigned char> RGBA2BGRA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size());

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            Output.push_back(Input.at(i+2));
            Output.push_back(Input.at(i+1));
            Output.push_back(Input.at(i));
            Output.push_back(Input.at(i+3));

        }

        return Output;
    }

    std::vector<unsigned char> BGRA2Gray(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/4);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            float Green = (float)Input.at(i) * 0.114f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Red = (float)Input.at(i+2) * 0.299f;
            Output.push_back((unsigned int)(Green + Blue + Red));
        }

        return Output;
    }

    std::vector<unsigned char> BGRA2GrayA(const std::vector<unsigned char> &Input)
    {
        std::vector<unsigned char> Output;
        Output.reserve(Input.size()/2);

        for (unsigned int i = 0; i < Input.size(); i += 4 )
        {
            float Green = (float)Input.at(i) * 0.114f;
            float Blue = (float)Input.at(i+1) * 0.587f;
            float Red = (float)Input.at(i+2) * 0.299f;
            Output.push_back((unsigned int)(Green + Blue + Red));
            Output.push_back(Input.at(i+3));
        }

        return Output;
    }
}