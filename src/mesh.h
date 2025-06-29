#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Mesh {
public:
    Mesh();
    void setVertices(const std::vector<float>& data);
    void render() const;
    GLuint VAO, VBO;

private:
    std::vector<float> vertices;
    GLsizei vertexCount = 0;
    void initBuffers();  // Needed to upload data to GPU
};