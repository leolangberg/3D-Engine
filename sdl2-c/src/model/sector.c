#include "sector.h"
#include "vector.h"
#include <stdint.h>
#include <stdlib.h>


Wall wall_create(const Vector* v1, const Vector* v2, const Vector* v3, const Vector* v4, uint32_t color) {
   
    Wall w;
    w.wall.matrix[0][0] = v1->x;
    w.wall.matrix[0][1] = v1->y;
    w.wall.matrix[0][2] = v1->z;
    w.wall.matrix[0][3] = 1;
    w.wall.matrix[1][0] = v2->x;
    w.wall.matrix[1][1] = v2->y;
    w.wall.matrix[1][2] = v2->z;
    w.wall.matrix[1][3] = 1;
    w.wall.matrix[2][0] = v3->x;
    w.wall.matrix[2][1] = v3->y;
    w.wall.matrix[2][2] = v3->z;
    w.wall.matrix[2][3] = 1;
    w.wall.matrix[3][0] = v4->x;
    w.wall.matrix[3][1] = v4->y;
    w.wall.matrix[3][2] = v4->z;
    w.wall.matrix[3][3] = 1;

    w.n = *vector_cross_product(vector_sub(v1, v2), vector_sub(v1, v4));
    w.color = color;

    return w;
}

    