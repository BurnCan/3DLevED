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

            //// Right border
            if (verticalWalls[z][x + 1]) {
                Map::MapObject rightWallObj("depthWall", "DepthWall",
                    basePos + glm::vec3(cellSize / 2.0f, 0.5f, 0.0f),
                    glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
                    shaderBase + ".vert", shaderBase + ".frag");
                rightWallObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
                mapBuffer.addObject(rightWallObj);
            }

            // Bottom border
            if (horizontalWalls[z + 1][x]) {
                Map::MapObject bottomWallObj("widthWall", "WidthWall",
                    basePos + glm::vec3(0.0f, 0.5f, cellSize / 2.0f),
                    glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
                    shaderBase + ".vert", shaderBase + ".frag");
                bottomWallObj.mesh = generateMeshForType("Cube", 1.0f); // Assign mesh
                mapBuffer.addObject(bottomWallObj);
            }

            // Top border (z = 0 row)
            for (int x = 0; x < width; ++x) {
                glm::vec3 pos = glm::vec3(x * cellSize, 0.5f, 0.0f - (cellSize / 2.0f));
                Map::MapObject topWallObj("topWall", "WidthWall", pos,
                    glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
                    shaderBase + ".vert", shaderBase + ".frag");
                topWallObj.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(topWallObj);
            }

            // Left border (x = 0 column)
            for (int z = 0; z < depth; ++z) {
                glm::vec3 pos = glm::vec3(0.0f - (cellSize / 2.0f), 0.5f, z * cellSize);
                Map::MapObject leftWallObj("leftWall", "DepthWall", pos,
                    glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
                    shaderBase + ".vert", shaderBase + ".frag");
                leftWallObj.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(leftWallObj);
            }
        }
    }

    std::cout << "Maze generated in mazeGen.cpp: " << width << " x " << depth << std::endl;
}

