#include "voxel.h"

void VoxelMap::resize(int w, int h, int d) {
    width = w;
    height = h;
    depth = d;

    voxels = std::vector<std::vector<std::vector<Voxel>>>(
        depth, std::vector<std::vector<Voxel>>(
            height, std::vector<Voxel>(
                width, Voxel{VoxelType::Empty, "basic"}  // Default values
            )
        )
    );
}

void VoxelMap::clear() {
    for (auto& layer : voxels)
        for (auto& row : layer)
            for (auto& voxel : row)
                voxel = Voxel{VoxelType::Empty, "basic"};
}

