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




void GenerateMaze(Map& mapBuffer, int width, int depth, float cellSize,
                  float floorHeight, const std::string& shaderBase,
                  bool randomOpenSpaces) {
    mapBuffer.clear();

    std::vector<std::vector<bool>> visited(depth, std::vector<bool>(width, false));
    std::vector<std::vector<bool>> horizontalWalls(depth + 1, std::vector<bool>(width, true));
    std::vector<std::vector<bool>> verticalWalls(depth, std::vector<bool>(width + 1, true));

    carveMaze(0, 0, width, depth, visited, verticalWalls, horizontalWalls);

    if (randomOpenSpaces) {
        int extraRemovals = (width * depth) / 5;

        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> xDist(0, width - 1);
        std::uniform_int_distribution<int> zDist(0, depth - 1);
        std::uniform_int_distribution<int> dirDist(0, 3); // up, right, down, left

        for (int i = 0; i < extraRemovals; ++i) {
            int x = xDist(rng);
            int z = zDist(rng);
            int dir = dirDist(rng);

            if (dir == 0 && z > 0) {
                horizontalWalls[z][x] = false; // top
            } else if (dir == 1 && x < width - 1) {
                verticalWalls[z][x + 1] = false; // right
            } else if (dir == 2 && z < depth - 1) {
                horizontalWalls[z + 1][x] = false; // bottom
            } else if (dir == 3 && x > 0) {
                verticalWalls[z][x] = false; // left
            }
        }
    }

    float offsetX = -(width * cellSize) / 2.0f + cellSize / 2.0f;
    float offsetZ = -(depth * cellSize) / 2.0f + cellSize / 2.0f;

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            glm::vec3 basePos = glm::vec3(x * cellSize + offsetX, floorHeight, z * cellSize + offsetZ);

            Map::MapObject floorObj("floor", "Floor", basePos, glm::vec3(0.0f),
                glm::vec3(1.0f, 0.1f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
            floorObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(floorObj);

            if (verticalWalls[z][x + 1]) {
                glm::vec3 wallPos = basePos + glm::vec3(cellSize / 2.0f, 0.5f, 0.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("depthWall", "DepthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(0.1f, 1.0f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

            if (horizontalWalls[z + 1][x]) {
                glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, cellSize / 2.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("widthWall", "WidthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(1.0f, 1.0f, 0.1f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

            if (z == 0 && horizontalWalls[z][x]) {
                glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, -cellSize / 2.0f);
                wallPos.y = floorHeight + 0.5f;
                Map::MapObject wall("topWall", "WidthWall", wallPos, glm::vec3(0.0f),
                    glm::vec3(1.0f, 1.0f, 0.1f), shaderBase + ".vert", shaderBase + ".frag");
                wall.mesh = generateMeshForType("Cube", 1.0f);
                mapBuffer.addObject(wall);
            }

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
