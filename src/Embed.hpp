#pragma once

#include <string>
#include <vector>

#include <GLFW/glfw3.h>

void SetWindowsIcon(GLFWwindow* Window);
std::vector<uint8_t> GetResource(int id);