
#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>

/**
* Basic structure for all polygon variants.
* State tells existance of object and num_verts tells number of vertices in object.
* A vector is placed for the center of the object, along with a velocity vector.
* A list of vertices is then represented as a Matrix (for calculation).
*/
typedef struct{
    int state;
    int num_verts;
    Vector* center;
    Vector* velocity;
    Matrix* vlist;
    uint32_t color;

}Polygon;

/**
* Creates a object polygon structure that represents a Triangle.
*/
Polygon* object_create_triangle();



#endif