// mazeGen.h
#ifndef MAZE_GEN_H
#define MAZE_GEN_H

#include "map.h"

// Generates a maze and fills the provided map buffer.
// You can pass shader name and cell size.
void GenerateMaze(Map& mapBuffer, int width, int depth, float cellSize, float floorHeight, const std::string& shaderBase);

#endif
