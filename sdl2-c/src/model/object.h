
#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>
#include "../integration/io.h"

#define OBJECT_SPEED 1

/**
* Declares all type of objects that can exist.
* VECTOR for vectors, POLYGON for 2D polygons and POLYGON3D for 3D polygons.
*/
typedef enum{
    VECTOR = 0,
    POLYGON = 1,
    POLYGON3D = 2
}ObjectType;

/**
* Basic structure for all polygon variants.
* num_verts tells number of vertices in object.
* A vector is placed for the center of the object, along with a velocity vector.
* A list of vertices is then represented as a Matrix (for calculation).
*/
typedef struct{
    int num_verts;
    Vector* center;
    Vector* velocity;
    Matrix* vertice_matrix;
}Polygon;

/**
* Structure for 3D polygons (e.g. Cube).
* A 3D polygon is built up of a collection of 2D polygons, therefore it needs
* to keep track of number of faces and hold a list of all faces (2D polygons) used.
* All vertices are placed out relative to the center and the radius as the distance.
*/
typedef struct{
    int num_faces;
    Polygon** face_list;
    Vector* center;
    float radius;
    Vector* velocity;
}Polygon3D;

/**
* Template Object Structure.
* Can take on the form of a Vector, Polygon or Polygon3D.
*/
typedef struct{
    ObjectType type;
    union {
        Polygon3D* polygon3D;
        Polygon* polygon;
        Vector* vector;
    };
}Object;

/**
* Creates an object by taking in the ENUM TYPE of object together with the actual
* object structure.
* @param enum_type type of object.
* @param structure actual object struct.
*/
Object* object_create_object(int enum_type, void* structure);

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
* Determines type of object the acts accordingly.
*/
void object_io(IO* io, Object* object);


/**
* Universal Object update function that determines type of object and acts accordingly.
* Calls objects own specific update function pointer.
*/
void object_update(Object* object);


/**
* Universal Draw function that determines type of object and acts accordingly.
* @param pixelmap uint32_t array of pixels that represent the display.
* @param object object to be drawn.
* @param distance distance to object (for polygon3D only).
* @param color uint32_t RGBA color.
*/
void object_draw(uint32_t* pixelmap, Object* object, float distance, uint32_t color);

/**
* Rotates object with give angle (radians).
* Performs matrix multiplication with all 3 rotational matrices (R = Rz * Ry * Rx);
*/
void object_polygon3d_rotate(Polygon3D* object, float angle_radian);

/**
* Copies given polygon.
*/
Polygon* object_polygon_copy(const Polygon* original);

/**
* Copies given 3D polygon.
*/
Polygon3D* object_polygon3d_copy(const Polygon3D* original);


#endif