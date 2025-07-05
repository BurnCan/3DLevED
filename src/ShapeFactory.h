// ShapeFactory.h
#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include "mesh.h"
Mesh createPyramid(float size);
Mesh createCube(float size);
Mesh createSphere(float radius, int sectors, int stacks);

#endif
