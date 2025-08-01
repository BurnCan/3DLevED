// ShapeFactory.cpp
#include "ShapeFactory.h"
#include <cmath>
#include <ostream>
#include <iostream>

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
// Utility to reuse cube mesh
const Mesh& getUnitCubeMesh() {
    static Mesh unitCube = createCube(1.0f);
    return unitCube;
}

////////
Mesh createWidthWall(float size) {
    Mesh mesh;
    mesh = createCube(size);  // Reuse cube geometry
    return mesh;
}

Mesh createDepthWall(float size) {
    Mesh mesh;
    mesh = createCube(size);  // Reuse cube geometry
    return mesh;
}

Mesh createFloorTile(float size) {
    Mesh mesh;
    mesh = createCube(size);  // Reuse cube geometry
    return mesh;
}
/////////

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

Mesh createPyramid(float size) {
    Mesh mesh;
    std::vector<float> vertices;

    float h = size;             // height
    float half = size / 2.0f;   // half base width

    glm::vec3 top(0.0f, h, 0.0f);

    // Base (two triangles)
    glm::vec3 v0(-half, 0.0f, -half);
    glm::vec3 v1( half, 0.0f, -half);
    glm::vec3 v2( half, 0.0f,  half);
    glm::vec3 v3(-half, 0.0f,  half);

    glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
    // Triangle 1
    vertices.insert(vertices.end(), {
        v0.x, v0.y, v0.z, baseNormal.x, baseNormal.y, baseNormal.z,
        v1.x, v1.y, v1.z, baseNormal.x, baseNormal.y, baseNormal.z,
        v2.x, v2.y, v2.z, baseNormal.x, baseNormal.y, baseNormal.z
    });
    // Triangle 2
    vertices.insert(vertices.end(), {
        v2.x, v2.y, v2.z, baseNormal.x, baseNormal.y, baseNormal.z,
        v3.x, v3.y, v3.z, baseNormal.x, baseNormal.y, baseNormal.z,
        v0.x, v0.y, v0.z, baseNormal.x, baseNormal.y, baseNormal.z
    });

    // Helper to compute normal for side triangles
    auto computeNormal = [](glm::vec3 a, glm::vec3 b, glm::vec3 c) -> glm::vec3 {
        return glm::normalize(glm::cross(b - a, c - a));
    };

    // Side 1 (v0, v1, top)
    glm::vec3 n1 = computeNormal(v0, v1, top);
    vertices.insert(vertices.end(), {
        v0.x, v0.y, v0.z, n1.x, n1.y, n1.z,
        v1.x, v1.y, v1.z, n1.x, n1.y, n1.z,
        top.x, top.y, top.z, n1.x, n1.y, n1.z
    });

    // Side 2 (v1, v2, top)
    glm::vec3 n2 = computeNormal(v1, v2, top);
    vertices.insert(vertices.end(), {
        v1.x, v1.y, v1.z, n2.x, n2.y, n2.z,
        v2.x, v2.y, v2.z, n2.x, n2.y, n2.z,
        top.x, top.y, top.z, n2.x, n2.y, n2.z
    });

    // Side 3 (v2, v3, top)
    glm::vec3 n3 = computeNormal(v2, v3, top);
    vertices.insert(vertices.end(), {
        v2.x, v2.y, v2.z, n3.x, n3.y, n3.z,
        v3.x, v3.y, v3.z, n3.x, n3.y, n3.z,
        top.x, top.y, top.z, n3.x, n3.y, n3.z
    });

    // Side 4 (v3, v0, top)
    glm::vec3 n4 = computeNormal(v3, v0, top);
    vertices.insert(vertices.end(), {
        v3.x, v3.y, v3.z, n4.x, n4.y, n4.z,
        v0.x, v0.y, v0.z, n4.x, n4.y, n4.z,
        top.x, top.y, top.z, n4.x, n4.y, n4.z
    });

    mesh.setVertices(vertices);
    return mesh;
}

Mesh createHexPrism(float radius, float height) {
    Mesh mesh;
    std::vector<float> vertices;

    float halfHeight = height / 2.0f;
    const int sides = 6;
    const float angleStep = 2.0f * M_PI / sides;

    // Generate bottom and top circle points
    std::vector<glm::vec3> bottom;
    std::vector<glm::vec3> top;

    for (int i = 0; i < sides; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        bottom.emplace_back(x, -halfHeight, z);
        top.emplace_back(x, halfHeight, z);
    }

    glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
    glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 down(0.0f, -1.0f, 0.0f);

    // Bottom face
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        vertices.insert(vertices.end(), {
            bottomCenter.x, bottomCenter.y, bottomCenter.z, down.x, down.y, down.z,
            bottom[next].x, bottom[next].y, bottom[next].z, down.x, down.y, down.z,
            bottom[i].x, bottom[i].y, bottom[i].z, down.x, down.y, down.z,
        });
    }

    // Top face
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        vertices.insert(vertices.end(), {
            topCenter.x, topCenter.y, topCenter.z, up.x, up.y, up.z,
            top[i].x, top[i].y, top[i].z, up.x, up.y, up.z,
            top[next].x, top[next].y, top[next].z, up.x, up.y, up.z,
        });
    }

    // Side faces
    for (int i = 0; i < sides; ++i) {
        int next = (i + 1) % sides;
        glm::vec3 v0 = bottom[i];
        glm::vec3 v1 = bottom[next];
        glm::vec3 v2 = top[next];
        glm::vec3 v3 = top[i];

        // Calculate normal using cross product
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v3 - v0));

        vertices.insert(vertices.end(), {
            v0.x, v0.y, v0.z, normal.x, normal.y, normal.z,
            v1.x, v1.y, v1.z, normal.x, normal.y, normal.z,
            v2.x, v2.y, v2.z, normal.x, normal.y, normal.z,

            v0.x, v0.y, v0.z, normal.x, normal.y, normal.z,
            v2.x, v2.y, v2.z, normal.x, normal.y, normal.z,
            v3.x, v3.y, v3.z, normal.x, normal.y, normal.z,
        });
    }

    mesh.setVertices(vertices);
    return mesh;
}

Mesh ShapeFactory::createShape(const std::string& type) {
    if (type == "Cube") {
        return createCube(1.0f);
    } else if (type == "Sphere") {
        return createSphere(1.0f, 16, 16);
    } else if (type == "Pyramid") {
        return createPyramid(1.0f);
    } else if (type == "HexPrism") {
        return createHexPrism(0.5f, 1.0f);  // Example: radius = 0.5, height = 1.0
    } else {
        std::cerr << "Unknown shape type: " << type << std::endl;
        return Mesh();  // empty mesh
    }
}
