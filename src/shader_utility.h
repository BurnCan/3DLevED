#pragma once
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>

extern GLuint shaderProgram; // Declare it for external use
unsigned int createShaderProgramFromFile(const std::string& vertexPath, const std::string& fragmentPath);
void reloadShaderProgram();
void renderShaderEditor(const std::filesystem::path& shaderDir, const glm::mat4& mvp);
