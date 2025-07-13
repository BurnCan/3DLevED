#pragma once
#include "map.h"  // for Map
#include "voxel.h"


void AddVoxelToMap(const Voxel& voxel, int x, int y, int z, float size, Map& map);
void GenerateVoxelObjects(const VoxelMap& vmap, Map& map);
