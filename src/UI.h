#pragma once

#include <string>
#include "map.h"
#include "shader_utility.h"

class Map;
struct GLFWwindow;

namespace UI {
    void Init(GLFWwindow* window);

    // Update these to match definitions in UI.cpp
    void RenderMainMenuBar(Map& mapBuffer, GLFWwindow* window);
    void RenderMapEditor(Map& mapBuffer);  // Add Map& parameter
    void RenderMazeGenerator(Map& mapBuffer);
    void RenderShaderUtility(const glm::mat4& mvp);
    void RenderCameraDebugWindow();


    void Shutdown();

    // Optional: Expose accessors if needed
    Map& GetMapBuffer();
    void ClearMapBuffer();
    bool LoadMapIntoBuffer(const std::string& path);
    bool SaveBufferToFile(const std::string& path);
}
