#pragma once

#include <string>

class Map;
struct GLFWwindow;

namespace UI {
    void Init(GLFWwindow* window);
    void RenderMainMenuBar(Map& currentMap, GLFWwindow* window);
    void Shutdown();
}