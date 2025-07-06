#include "mazeGen.h"
#include <glm/glm.hpp>
#include <iostream>

void GenerateMaze(Map& mapBuffer, int width, int depth, float cellSize, const std::string& shaderBase) {
    mapBuffer.clear();  // Start fresh

    // Wall flags: all walls up initially
    std::vector<std::vector<bool>> horizontalWalls(depth + 1, std::vector<bool>(width, true));
    std::vector<std::vector<bool>> verticalWalls(depth, std::vector<bool>(width + 1, true));

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 basePos = glm::vec3(x * cellSize, 0.0f, z * cellSize);

            // Floor
            Map::MapObject floorObj("floor", "Floor", basePos, glm::vec3(0.0f),
                glm::vec3(1.0f, 0.1f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
            floorObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
            mapBuffer.addObject(floorObj);

            // Right wall
            if (verticalWalls[z][x + 1]) {
                Map::MapObject rightWallObj("depthWall", "DepthWall",
                    basePos + glm::vec3(cellSize / 2.0f, 0.5f, 0.0f),
                    glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
                    shaderBase + ".vert", shaderBase + ".frag");
                rightWallObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
                mapBuffer.addObject(rightWallObj);
            }

            // Bottom wall
            if (horizontalWalls[z + 1][x]) {
                Map::MapObject bottomWallObj("widthWall", "WidthWall",
                    basePos + glm::vec3(0.0f, 0.5f, cellSize / 2.0f),
                    glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
                    shaderBase + ".vert", shaderBase + ".frag");
                bottomWallObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
                mapBuffer.addObject(bottomWallObj);
            }
        }
    }

    // Right border
    for (int z = 0; z < depth; ++z) {
        glm::vec3 pos = glm::vec3(width * cellSize, 0.5f, z * cellSize);
        Map::MapObject rightBorderObj("rightWall", "DepthWall", pos,
            glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
            shaderBase + ".vert", shaderBase + ".frag");
        rightBorderObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
        mapBuffer.addObject(rightBorderObj);
    }

    // Bottom border
    for (int x = 0; x < width; ++x) {
        glm::vec3 pos = glm::vec3(x * cellSize, 0.5f, depth * cellSize);
        Map::MapObject bottomBorderObj("bottomWall", "WidthWall", pos,
            glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
            shaderBase + ".vert", shaderBase + ".frag");
        bottomBorderObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
        mapBuffer.addObject(bottomBorderObj);
    }

    std::cout << "Maze generated in mazeGen.cpp: " << width << " x " << depth << std::endl;
}

