#include "object.h"
#include "matrix.h"
#include <stdlib.h>

/**
* Creates a object polygon structure that represents a Triangle.
*/
Polygon* object_create_triangle() {

    Polygon* triangle = malloc(sizeof(Polygon));
    triangle->state = 1;
    triangle->num_verts = 3;
    triangle->center = vector_create(175, 175, 0);
    triangle->velocity = vector_create(0, 0, 0);

    Matrix* m = malloc(sizeof(Matrix));
    m->matrix[0][0] = 200;
    m->matrix[0][1] = 200;
    m->matrix[0][2] = 0;
    m->matrix[0][3] = 1;
    m->matrix[1][0] = 150;
    m->matrix[1][1] = 150;
    m->matrix[1][2] = 0;
    m->matrix[1][3] = 1;
    m->matrix[2][0] = 150;
    m->matrix[2][1] = 200;
    m->matrix[2][2] = 0;
    m->matrix[2][3] = 1;

    triangle->vlist = m;

    return triangle;
    
 }