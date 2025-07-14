#include "voxelRenderer.h"
#include "map.h"
#include "voxel.h"

#include "ShapeFactory.h"
#include <glm/glm.hpp>

void AddVoxelToMap(const Voxel& voxel, int x, int y, int z, float size, Map& map) {
    if (voxel.type == VoxelType::Empty)
        return;

    std::string name = "Voxel_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
    std::string type = "HexPrism"; // You can change this based on voxel.type if desired

    glm::vec3 position(x * size, y * size, z * size);
    glm::vec3 rotation(0.0f); // Default rotation
    glm::vec3 scale(size);    // Uniform scale

    std::string vertShader = voxel.shaderBase + ".vert";
    std::string fragShader = voxel.shaderBase + ".frag";

    Map::MapObject obj(name, type, position, rotation, scale, vertShader, fragShader);

    map.addObject(obj);
}


void GenerateVoxelObjects(const VoxelMap& vmap, Map& map) {
    map.clear();
    for (int z = 0; z < vmap.depth; ++z)
        for (int y = 0; y < vmap.height; ++y)
            for (int x = 0; x < vmap.width; ++x)
                AddVoxelToMap(vmap.voxels[z][y][x], x, y, z, vmap.voxelSize, map);
}
