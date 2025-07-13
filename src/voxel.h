#pragma once
#include <vector>
#include <string>
#include "map.h"


enum class VoxelType { Empty, Solid, Floor, Water };

struct Voxel {
    VoxelType type = VoxelType::Empty;
    std::string shaderBase = "basic";
};

struct VoxelMap {
    int width = 0;
    int height = 1;
    int depth = 0;
    float voxelSize = 1.0f;

    std::vector<std::vector<std::vector<Voxel>>> voxels;

    void resize(int w, int h, int d);
    void clear();
};

void GenerateVoxelObjects(const VoxelMap& vmap, Map& map);
