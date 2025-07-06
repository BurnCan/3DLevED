#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>



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

Mesh generateMeshForType(const std::string& type, float scale);
