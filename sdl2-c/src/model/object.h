
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

typedef struct{
    int num_faces;
    Polygon** face_list;
    Vector* center;
    float radius;
    Vector* velocity;

    void (*update)();
}Polygon3D;

/**
* Creates a object polygon structure that represents a Triangle.
* @param v vertice.
*/
Polygon* object_create_triangle(const Vector* v1, const Vector* v2, const Vector* v3);


/**
* Creates a polygon square based on 4 points (vectors).
* @param v1 top left point.
* @param v2 top right point.
* @param v3 bottom right point.
* @param v4 bottom left point.
*/
Polygon* object_create_square(const Vector* v1, const Vector* v2, const Vector* v3, const Vector* v4);

/**
* Creates a 3D cube consisting of 6 squares. 
* @param position_center represents the singular center point of the cube.
* @param radius represent distance from center point to each face (i.e radius even though its not a circle).
* List of faces is represented as follows:
* [0] = front.
* [1] = right.
* [2] = back.
* [3] = left.
* [4] = top.
* [5] = bottom.
*/
Polygon3D* object_create_cube(Vector* position_center, float radius);

/**
* Handles object related IO input (Object controls).
*/
void object_io(IO* io, Polygon3D* object);


/**
* Applies perspective on a 3D object the same as an object is scaled.
* That is, by transforming the matrix to the origin,
* peform scaling according to camera distance,
* transform back to original position.
* @param object3d object that perspective scaling is applied on.
* @param distance represents camera distance.
*/
void object_perspective_3d(Polygon3D* object3d, float distance);


/**
* Creates a translation matrix out of the velocity vector and then 
* performs matrix multiplication with current vertices to get new position.
* Also performs vector addition to update object center.
*/
void object_update(Polygon* object);

/**
* Updates 3D object by performing individual updates for all faces.
* @param object3d 3D object to be update.
* @param distance camera distance.
*/
void object_update_3d(Polygon3D* object3d);

/**
* Draws object onto the pixelmap screen by retrieving all vertices
* and then drawing out the lines between them.
*/
void object_draw(uint32_t* pixelmap, Polygon* object, uint32_t color);

/**
* Creates a copy of original object then performs perspective calculation on said copy.
* This copy (with correct perspective) is then sent to draw out each polygon inside this 3D polygon 
* individually.
*/
void object_draw_3d(uint32_t* pixelmap, Polygon3D* object, float distance, uint32_t color);

/**
* Rotates object with give angle (radians).
* Performs matrix multiplication with all 3 rotational matrices (R = Rz * Ry * Rx);
*/
void object_rotate_3d(Polygon3D* object, float angle_radian);

/**
* Copies given polygon.
*/
Polygon* object_polygon_copy(const Polygon* original);

/**
* Copies given 3D polygon.
*/
Polygon3D* object_polygon3d_copy(const Polygon3D* original);


#endif