#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Object.hpp"

extern glm::ivec2 g_WindowSize;
extern std::vector<bool> g_KeyboardState;
extern glm::vec2 g_CursorPos;
extern glm::ivec2 g_GameSize;
extern glm::ivec2 g_ActualGameSize;

extern bool g_WindowHovered;

extern bool g_WindowResizeHint;
extern bool g_GameResizeHint;
extern bool g_GameUpdateHint;
extern bool g_GameRedrawHint;

extern float g_DeltaTIme;