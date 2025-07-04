#pragma once
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "imgui.h"

// External state variables
extern GLuint shaderProgram; // Declare it for external use
extern std::vector<std::filesystem::path> shaders;
extern std::filesystem::path currentShaderPath;
extern std::vector<std::filesystem::path> shaders;
extern int selectedIndex;
extern std::vector<char> shaderBuffer;



// Functions
std::vector<std::filesystem::path> listShaderFiles(const std::filesystem::path& dir);
bool saveShaderSource(const std::filesystem::path& filePath, const std::string& content);

GLuint loadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
GLuint createShaderProgramFromFile(const std::string& vertFile, const std::string& fragFile);
int TextEditCallback(ImGuiInputTextCallbackData* data);


//unsigned int createShaderProgramFromFile(const std::string& vertexPath, const std::string& fragmentPath);
void reloadShaderProgram();
void renderShaderEditor(const std::filesystem::path& shaderDir, const glm::mat4& mvp);
