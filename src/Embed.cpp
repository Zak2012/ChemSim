#include "Embed.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

#include "embed/Res.rc"

static const std::vector<std::string> PathList = {
    "",
    "/Chemsim.ico",
    "/Chemsim.png",
    "/Basic.vert",
    "/Basic.frag",
    "/Sprite.vert",
    "/Sprite.frag",
    "/Circle.vert",
    "/Circle.frag",
    "/Text.vert",
    "/Text.frag",
    "/OpenSans-Regular.ttf",
};

std::vector<uint8_t> ReadBinaryFile(std::string Path)
{
    std::cout << "Loading " + Path + "\n";

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

#ifndef __EMSCRIPTEN__

#undef WINVER
#define WINVER NTDDI_WIN7

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7

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
        return ReadBinaryFile(PathList[id]);
    }

#endif
