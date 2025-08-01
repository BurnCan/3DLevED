#include "mesh.h"
#include <iostream>
#include "ShapeFactory.h"
#include <unordered_map>
#include <functional>
Mesh::Mesh() : VAO(0), VBO(0), vertexCount(0) {}

void Mesh::setVertices(const std::vector<float>& data) {
    vertices = data;
    vertexCount = static_cast<GLsizei>(vertices.size() / 6); // 3 pos + 3 normal
    initBuffers();
}

void Mesh::initBuffers() {
    if (VAO == 0) glGenVertexArrays(1, &VAO);
    if (VBO == 0) glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Mesh generateMeshForType(const std::string& type, float scale) {
    static const std::unordered_map<std::string, std::function<Mesh(float)>> meshGenerators = {
        { "Cube",    createCube },
        { "Sphere",  [](float s) { return createSphere(s, 36, 18); } },
        { "Pyramid", createPyramid },
        { "WidthWall", createCube },     // Same mesh, scale changes later
        { "DepthWall", createCube },
        { "Floor", createCube }
    };

    auto it = meshGenerators.find(type);
    if (it != meshGenerators.end()) {
        return it->second(scale);
    } else {
        std::cerr << "Unknown shape type: " << type << std::endl;
        return Mesh(); // Return empty fallback mesh
    }
}

void Mesh::render() const {
    if (VAO == 0 || vertexCount == 0) return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

