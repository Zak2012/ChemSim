#include "Global.hpp"

#include <GLFW/glfw3.h>


glm::ivec2 g_WindowSize = {0,0};
std::vector<bool> g_KeyboardState(GLFW_KEY_LAST);
glm::vec2 g_CursorPos = {0.0f,0.0f};
glm::ivec2 g_GameSize = {0,0};
glm::ivec2 g_ActualGameSize = {0,0};

bool g_WindowHovered = true;

bool g_WindowResizeHint = true;
bool g_GameResizeHint = true;
bool g_GameUpdateHint = true;
bool g_GameRedrawHint = true;


float g_DeltaTIme = 0.0f;
