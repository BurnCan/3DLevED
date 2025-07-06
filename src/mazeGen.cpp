#include "mazeGen.h"
#include <glm/glm.hpp>
#include <iostream>

void GenerateMaze(Map& mapBuffer, int width, int depth, float cellSize, float floorHeight, const std::string& shaderBase) {
    mapBuffer.clear();  // Start fresh

    //for shifting the maze to center it (over x0 z0 for example)
    float offsetX = -((width - 1) * cellSize) / 2.0f;
    float offsetZ = -((depth - 1) * cellSize) / 2.0f;

    // Wall flags: all walls up initially
    std::vector<std::vector<bool>> horizontalWalls(depth + 1, std::vector<bool>(width, true));
    std::vector<std::vector<bool>> verticalWalls(depth, std::vector<bool>(width + 1, true));

    for (int z = 0; z < depth; ++z) {
    for (int x = 0; x < width; ++x) {
        glm::vec3 basePos = glm::vec3(x * cellSize + offsetX, floorHeight, z * cellSize + offsetZ);

        // === Floor ===
        Map::MapObject floorObj("floor", "Floor", basePos, glm::vec3(0.0f),
            glm::vec3(1.0f, 0.1f, 1.0f), shaderBase + ".vert", shaderBase + ".frag");
        floorObj.mesh = generateMeshForType("Cube", 1.0f);
        mapBuffer.addObject(floorObj);

        // === Right Wall ===
        if (verticalWalls[z][x + 1]) {
            glm::vec3 wallPos = basePos + glm::vec3(cellSize / 2.0f, 0.5f, 0.0f);
            wallPos.y = floorHeight + (0.5);
            Map::MapObject rightWallObj("depthWall", "DepthWall", wallPos,
                glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
                shaderBase + ".vert", shaderBase + ".frag");
            rightWallObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(rightWallObj);
        }

        // === Bottom Wall ===
        if (horizontalWalls[z + 1][x]) {
            glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, cellSize / 2.0f);
            wallPos.y = floorHeight + (0.5);
            Map::MapObject bottomWallObj("widthWall", "WidthWall", wallPos,
                glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
                shaderBase + ".vert", shaderBase + ".frag");
            bottomWallObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(bottomWallObj);
        }

        // === Left Wall (for x == 0) ===
        if (x == 0 && verticalWalls[z][x]) {
            glm::vec3 wallPos = basePos + glm::vec3(-cellSize / 2.0f, 0.5f, 0.0f);
            wallPos.y = floorHeight + (0.5);
            Map::MapObject leftWallObj("leftWall", "DepthWall", wallPos,
                glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
                shaderBase + ".vert", shaderBase + ".frag");
            leftWallObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(leftWallObj);
        }

        // === Top Wall (for z == 0) ===
        if (z == 0 && horizontalWalls[z][x]) {
            glm::vec3 wallPos = basePos + glm::vec3(0.0f, 0.5f, -cellSize / 2.0f);
            wallPos.y = floorHeight + (0.5);
            Map::MapObject topWallObj("topWall", "WidthWall", wallPos,
                glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
                shaderBase + ".vert", shaderBase + ".frag");
            topWallObj.mesh = generateMeshForType("Cube", 1.0f);
            mapBuffer.addObject(topWallObj);
        }
    }
}

    // === Border Walls ===

    //// Right border
    //for (int z = 0; z < depth; ++z) {
        //glm::vec3 pos = glm::vec3(width * cellSize, floorHeight + 0.5f, z * cellSize);
        //Map::MapObject rightWall("rightWall", "DepthWall", pos,
            //glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
            //shaderBase + ".vert", shaderBase + ".frag");
        //rightWall.mesh = generateMeshForType("Cube", 1.0f);
        //mapBuffer.addObject(rightWall);
    //}

    //// Bottom border
    //for (int x = 0; x < width; ++x) {
        //glm::vec3 pos = glm::vec3(x * cellSize, floorHeight + 0.5f, depth * cellSize);
        //Map::MapObject bottomWall("bottomWall", "WidthWall", pos,
            //glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
            //shaderBase + ".vert", shaderBase + ".frag");
        //bottomWall.mesh = generateMeshForType("Cube", 1.0f);
        //mapBuffer.addObject(bottomWall);
    //}

    //// Top border
    //for (int x = 0; x < width; ++x) {
        //glm::vec3 pos = glm::vec3(x * cellSize, floorHeight + 0.5f, -cellSize / 2.0f);
        //Map::MapObject topWall("topWall", "WidthWall", pos,
            //glm::vec3(0.0f), glm::vec3(1.0f, 1.0f, 0.1f),
            //shaderBase + ".vert", shaderBase + ".frag");
        //topWall.mesh = generateMeshForType("Cube", 1.0f);
        //mapBuffer.addObject(topWall);
    //}

    //// Left border
    //for (int z = 0; z < depth; ++z) {
        //glm::vec3 pos = glm::vec3(-cellSize / 2.0f, floorHeight + 0.5f, z * cellSize);
        //Map::MapObject leftWall("leftWall", "DepthWall", pos,
            //glm::vec3(0.0f), glm::vec3(0.1f, 1.0f, 1.0f),
            //shaderBase + ".vert", shaderBase + ".frag");
        //leftWall.mesh = generateMeshForType("Cube", 1.0f);
        //mapBuffer.addObject(leftWall);
    //}

    std::cout << "Maze generated in mazeGen.cpp: " << width << " x " << depth << std::endl;
}


