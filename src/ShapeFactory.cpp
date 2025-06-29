// ShapeFactory.cpp
#include "ShapeFactory.h"
#include <cmath>

Mesh createCube(float size) {
    Mesh mesh;

    float h = size / 2.0f;
    std::vector<float> vertices = {
        // Front face (+Z)
        -h, -h,  h,  0.0f, 0.0f, 1.0f,
         h, -h,  h,  0.0f, 0.0f, 1.0f,
         h,  h,  h,  0.0f, 0.0f, 1.0f,
        -h, -h,  h,  0.0f, 0.0f, 1.0f,
         h,  h,  h,  0.0f, 0.0f, 1.0f,
        -h,  h,  h,  0.0f, 0.0f, 1.0f,

        // Back face (-Z)
        -h, -h, -h,  0.0f, 0.0f, -1.0f,
        -h,  h, -h,  0.0f, 0.0f, -1.0f,
         h,  h, -h,  0.0f, 0.0f, -1.0f,
        -h, -h, -h,  0.0f, 0.0f, -1.0f,
         h,  h, -h,  0.0f, 0.0f, -1.0f,
         h, -h, -h,  0.0f, 0.0f, -1.0f,

         // Left face (-X)
         -h, -h, -h, -1.0f, 0.0f, 0.0f,
         -h, -h,  h, -1.0f, 0.0f, 0.0f,
         -h,  h,  h, -1.0f, 0.0f, 0.0f,
         -h, -h, -h, -1.0f, 0.0f, 0.0f,
         -h,  h,  h, -1.0f, 0.0f, 0.0f,
         -h,  h, -h, -1.0f, 0.0f, 0.0f,

         // Right face (+X)
          h, -h, -h, 1.0f, 0.0f, 0.0f,
          h,  h,  h, 1.0f, 0.0f, 0.0f,
          h, -h,  h, 1.0f, 0.0f, 0.0f,
          h, -h, -h, 1.0f, 0.0f, 0.0f,
          h,  h, -h, 1.0f, 0.0f, 0.0f,
          h,  h,  h, 1.0f, 0.0f, 0.0f,

          // Top face (+Y)
          -h,  h, -h, 0.0f, 1.0f, 0.0f,
          -h,  h,  h, 0.0f, 1.0f, 0.0f,
           h,  h,  h, 0.0f, 1.0f, 0.0f,
          -h,  h, -h, 0.0f, 1.0f, 0.0f,
           h,  h,  h, 0.0f, 1.0f, 0.0f,
           h,  h, -h, 0.0f, 1.0f, 0.0f,

           // Bottom face (-Y)
           -h, -h, -h, 0.0f, -1.0f, 0.0f,
            h, -h,  h, 0.0f, -1.0f, 0.0f,
            h, -h, -h, 0.0f, -1.0f, 0.0f,
           -h, -h, -h, 0.0f, -1.0f, 0.0f,
           -h, -h,  h, 0.0f, -1.0f, 0.0f,
            h, -h,  h, 0.0f, -1.0f, 0.0f,
    };

    mesh.setVertices(vertices);  //  Use this instead of accessing mesh.vertices directly
    return mesh;
}

Mesh createSphere(float radius, int sectors, int stacks) {
    Mesh mesh;
    std::vector<float> vertices;

    for (int i = 0; i < stacks; ++i) {
        float stackAngle1 = M_PI * i / stacks;
        float stackAngle2 = M_PI * (i + 1) / stacks;

        float z1 = radius * cosf(stackAngle1);
        float z2 = radius * cosf(stackAngle2);

        float xy1 = radius * sinf(stackAngle1);
        float xy2 = radius * sinf(stackAngle2);

        for (int j = 0; j < sectors; ++j) {
            float sectorAngle1 = 2 * M_PI * j / sectors;
            float sectorAngle2 = 2 * M_PI * (j + 1) / sectors;

            glm::vec3 p1 = glm::vec3(xy1 * cosf(sectorAngle1), xy1 * sinf(sectorAngle1), z1);
            glm::vec3 p2 = glm::vec3(xy2 * cosf(sectorAngle1), xy2 * sinf(sectorAngle1), z2);
            glm::vec3 p3 = glm::vec3(xy1 * cosf(sectorAngle2), xy1 * sinf(sectorAngle2), z1);
            glm::vec3 p4 = glm::vec3(xy2 * cosf(sectorAngle2), xy2 * sinf(sectorAngle2), z2);

            // Triangle 1 (p1, p2, p3)
            vertices.insert(vertices.end(), {
                p1.x, p1.y, p1.z, p1.x / radius, p1.y / radius, p1.z / radius,
                p2.x, p2.y, p2.z, p2.x / radius, p2.y / radius, p2.z / radius,
                p3.x, p3.y, p3.z, p3.x / radius, p3.y / radius, p3.z / radius,
                });

            // Triangle 2 (p3, p2, p4)
            vertices.insert(vertices.end(), {
                p3.x, p3.y, p3.z, p3.x / radius, p3.y / radius, p3.z / radius,
                p2.x, p2.y, p2.z, p2.x / radius, p2.y / radius, p2.z / radius,
                p4.x, p4.y, p4.z, p4.x / radius, p4.y / radius, p4.z / radius,
                });
        }
    }

    mesh.setVertices(vertices);  //  Correct way to upload data
    return mesh;
}
