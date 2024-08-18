#include "Embed.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>

#undef WINVER
#define WINVER NTDDI_WIN10_19H1

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <windows.h>
#include <GLFW/glfw3native.h>

#include "../res/Res.rc"

std::string FTLLSC;
std::string GFLLSC;
std::string GLWLSC;
std::string XMLLSC;
std::string HFZLSC;
std::string GLMLSC;
std::string STBLSC;
std::string R2DLSC;
std::string B2DLSC;
std::string SELFLSC;

std::string BasicVert;
std::string BasicFrag;
std::string SpriteVert;
std::string SpriteFrag;
std::string CircleVert;
std::string CircleFrag;
std::string TextVert;
std::string TextFrag;

void SetWindowsIcon(GLFWwindow* Window)
{
    HICON icon = LoadIconA(GetModuleHandle(0), MAKEINTRESOURCE(IDI_AICON));

    HWND hWnd = glfwGetWin32Window(Window);

    SendMessageA(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessageA(hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
}

void LoadFileInResource()
{
    HMODULE handle = GetModuleHandle(NULL);
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_FTLLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        FTLLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_GFLLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        GFLLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_GLWLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        GLWLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_XMLLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        XMLLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_HFZLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        HFZLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_GLMLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        GLMLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_STBLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        STBLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_R2DLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        R2DLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_B2DLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        B2DLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_SELFLSC), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        SELFLSC = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }

    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_BSVSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        BasicVert = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_BSFSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        BasicFrag = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_SPVSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        SpriteVert = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_SPFSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        SpriteFrag = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_CRVSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        CircleVert = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_CRFSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        CircleFrag = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_TXVSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        TextVert = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
    {
        HRSRC rc = FindResourceA(handle, MAKEINTRESOURCE(IDR_TXFSDR), MAKEINTRESOURCE(TEXTFILE));
        HGLOBAL rcData = LoadResource(handle, rc);
        TextFrag = std::string(static_cast<const char*>(::LockResource(rcData)), SizeofResource(handle, rc));
    }
}
