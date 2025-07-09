#include "mazeGen.h"
#include "map.h"
#include "ShapeFactory.h"

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

static void carveMaze(int x, int z, int width, int depth,
                      std::vector<std::vector<bool>>& visited,
                      std::vector<std::vector<bool>>& verticalWalls,
                      std::vector<std::vector<bool>>& horizontalWalls) {
    visited[z][x] = true;

    std::vector<std::pair<int, int>> directions = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}  // Up, Right, Down, Left
    };

    std::shuffle(directions.begin(), directions.end(), std::mt19937{ std::random_device{}() });

    for (auto [dx, dz] : directions) {
        int nx = x + dx;
        int nz = z + dz;

        if (nx >= 0 && nx < width && nz >= 0 && nz < depth && !visited[nz][nx]) {
            if (dx == 1)       verticalWalls[z][x + 1] = false; // remove right wall
            else if (dx == -1) verticalWalls[z][x] = false;     // remove left wall
            else if (dz == 1)  horizontalWalls[z + 1][x] = false; // remove bottom wall
            else if (dz == -1) horizontalWalls[z][x] = false;     // remove top wall

            carveMaze(nx, nz, width, depth, visited, verticalWalls, horizontalWalls);
        }
    }
}

void GenerateMaze(Map& mapBuffer, int width, int depth, float cellSize, float floorHeight, const std::string& shaderBase) {
    mapBuffer.clear();

    // Maze wall layout
    std::vector<std::vector<bool>> visited(depth, std::vector<bool>(width, false));
    std::vector<std::vector<bool>> horizontalWalls(depth + 1, std::vector<bool>(width, true));
    std::vector<std::vector<bool>> verticalWalls(depth, std::vector<bool>(width + 1, true));

    carveMaze(0, 0, width, depth, visited, verticalWalls, horizontalWalls);

    // Offset to center maze over origin
    float offsetX = -(width * cellSize) / 2.0f + cellSize / 2.0f;
    float offsetZ = -(depth * cellSize) / 2.0f + cellSize / 2.0f;

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 basePos = glm::vec3(x * cellSize + offsetX, floorHeight, z * cellSize + offsetZ);

            // Floor tile
            Map::MapObject floorObj("floor", "Floor", basePos, glm::vec3(0.0f),
                glm::vec3(1.0f, 0.1f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
            floorObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(floorObj);

            // Right wall (shared between this and neighbor)
            if (verticalWalls[z][x + 1]) {
                glm::vec3 wallPos = basePos + glm::vec3(cellSize / 2.0f, 0.5f, 0.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("depthWall", "DepthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(0.1f, 1.0f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

            // Bottom wall
            if (horizontalWalls[z + 1][x]) {
                glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, cellSize / 2.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("widthWall", "WidthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(1.0f, 1.0f, 0.1f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

            // Top wall for first row
            if (z == 0 && horizontalWalls[z][x]) {
                glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, -cellSize / 2.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("topWall", "WidthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(1.0f, 1.0f, 0.1f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

            // Left wall for first column
            if (x == 0 && verticalWalls[z][x]) {
                glm::vec3 wallPos = basePos + glm::vec3(-cellSize / 2.0f, 0.5f, 0.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("leftWall", "DepthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(0.1f, 1.0f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }
        }
    }

    std::cout << "Maze generated: " << width << " x " << depth << std::endl;
}
