// ShapeFactory.h
#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include "mesh.h"

class ShapeFactory {
public:
    static Mesh createShape(const std::string& type);  // Ensure createShape is declared
};

Mesh createPyramid(float size);
Mesh createCube(float size);
Mesh createSphere(float radius, int sectors, int stacks);
Mesh createHexPrism(float radius, float height);

// Central mesh generation dispatch
Mesh generateMeshForType(const std::string& type, float scale);
// Reusable unit cube mesh for scalable shapes like walls/floors
const Mesh& getUnitCubeMesh();

#endif
