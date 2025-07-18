#include "Embed.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

#include "embed/Res.rc"

static std::vector<std::string> PathList;
// static std::vector<std::string> PathList = {
//     "",
//     "/Chemsim.ico",
//     "/Chemsim.png",
//     "/Basic.vert",
//     "/Basic.frag",
//     "/Sprite.vert",
//     "/Sprite.frag",
//     "/Circle.vert",
//     "/Circle.frag",
//     "/Text.vert",
//     "/Text.frag",
//     "/OpenSans-Regular.ttf",
// };

std::vector<uint8_t> ReadBinaryFile(std::string Path)
{
    std::ifstream In(Path, std::ios::binary);
    if ( In.fail() )
    {
        std::cout << "File " + Path + " not found\n";
    }

    // get its size:
    In.seekg(0, std::ios::end);
    std::streampos fileSize = In.tellg();
    In.seekg(0, std::ios::beg);

    std::vector<uint8_t> Buffer;
    Buffer.resize((int)fileSize);
    In.read((char *)Buffer.data(), (int)fileSize);
    In.close();
    return Buffer;
}

std::vector<std::string> SplitString(std::string s, const std::string &delimiter)
{
    std::vector<std::string> Result;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        Result.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    if ((pos = s.find(delimiter)) == std::string::npos)
    {
        Result.push_back(s);
    }
    return Result;
}


void GetEmbedPath()
{
    std::ifstream file("/Res.rc");

    float AfterInvoke  = false;
    PathList.push_back("");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("#ifdef RC_INVOKED") != std::string::npos)
            {
                AfterInvoke = true;
                continue;
            }
            if (AfterInvoke)
            {
                std::vector<std::string> String = SplitString(line, " ");
                if (String.size() == 3)
                {
                    std::string Path = String[2];
                    std::size_t Pos = Path.find_last_of("/");
                    std::string Filename = Path.substr(Pos,Path.size()-Pos-2);
                    PathList.push_back(Filename);
                    std::cout << Filename << std::endl;
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Error: Could not open file Res.rc" << std::endl;
    }
}

#ifndef __EMSCRIPTEN__

#include "windows.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

static HMODULE handle = GetModuleHandle(NULL);

void SetWindowsIcon(GLFWwindow* Window)
{
    HICON icon = LoadIconA(handle, MAKEINTRESOURCE(IDI_AICON));

    HWND hWnd = glfwGetWin32Window(Window);

    SendMessageA(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessageA(hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon);

}

std::vector<uint8_t> GetResource(int id)
{
    HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(id), RT_RCDATA);
    HGLOBAL rcData = LoadResource(handle, rc);
    uint8_t* data = (uint8_t*)LockResource(rcData);
    return std::vector<uint8_t>(data, data + SizeofResource(handle, rc));
}

#else
void SetWindowsIcon(GLFWwindow* Window){}
std::vector<uint8_t> GetResource(int id)
{
        if (PathList.size() == 0)
        {
            GetEmbedPath();
        }
        return ReadBinaryFile(PathList[id]);
    }

#endif
