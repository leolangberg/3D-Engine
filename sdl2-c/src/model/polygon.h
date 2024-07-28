#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>

typedef struct {
    Vector v[3];
    int color;
}Triangle;

Triangle triangle_create(const Vector* v1, const Vector* v2, const Vector* v3, int color);

void draw_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);
void draw_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);
void triangle_draw_2D(Triangle* triangle, uint32_t* pixelmap);

#endif