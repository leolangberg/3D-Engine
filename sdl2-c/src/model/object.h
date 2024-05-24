
#ifndef OBJECT_H
#define OBJECT_H

#define OBJECT_SPEED 1

#include "vector.h"
#include "matrix.h"
#include <stdint.h>
#include "../integration/io.h"

/**
* Basic structure for all polygon variants.
* State tells existance of object and num_verts tells number of vertices in object.
* A vector is placed for the center of the object, along with a velocity vector.
* A list of vertices is then represented as a Matrix (for calculation).
*/
typedef struct{
    int num_verts;
    Vector* center;
    Vector* velocity;
    Matrix* vertice_matrix;
    uint32_t color;

    void (*update)();

}Polygon;

/**
* Creates a object polygon structure that represents a Triangle.
* @param v vertice.
*/
Polygon* object_create_triangle(const Vector* v1, const Vector* v2, const Vector* v3);

/**
* Creates a object polygon structure that represents a Square.
* @param position_center vector position of square center (vertices will be generated 
*                        based on this point).
* @param width width of square.
* @param height of square.
*/
Polygon* object_create_square(Vector* position_center, float width, float height);

/**
* Handles object related IO input (Object controls).
*/
void object_io(IO* io, Polygon* object);

/**
* Creates a translation matrix out of the velocity vector and then 
* performs matrix multiplication with current vertices to get new position.
* Also performs vector addition to update object center.
*/
void object_update(Polygon* object);

/**
* Draws object onto the pixelmap screen by retrieving all vertices
* and then drawing out the lines between them.
*/
void object_draw(uint32_t* pixelmap, Polygon* object, uint32_t color);


#endif