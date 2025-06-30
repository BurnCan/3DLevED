#pragma once

#include <string>
#include "map.h"

class Map;
struct GLFWwindow;

namespace UI {
    void Init(GLFWwindow* window);
    void RenderMainMenuBar(Map& currentMap, GLFWwindow* window);
    void RenderMapEditor(Map& currentMap);
    void Shutdown();
}