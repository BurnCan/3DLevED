#pragma once

#include <string>
#include "map.h"
#include "shader_utility.h"

class Map;
struct GLFWwindow;

namespace UI {
    void Init(GLFWwindow* window);
    void RenderMainMenuBar(Map& currentMap, GLFWwindow* window);
    void RenderMapEditor(Map& currentMap);
    void RenderShaderUtility(const glm::mat4& mvp);
    void Shutdown();
}
