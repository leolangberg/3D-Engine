#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>
#include <stdio.h>


#define MAX_POINTS_PER_POLYGON 4
#define MAX_VERTICES_PER_OBJECT 24
#define MAX_POLYS_PER_OBJECT 6


typedef struct {
    Vector v[3];
    int color;
}Triangle;

typedef struct {
    int num_points;
    int vertex_list[MAX_POINTS_PER_POLYGON];
    Vector normal;

    int color;
    int shade;
    int shading;
    int two_sided;
    int visible;
    int active;
    int clipped;
    
}Polygon;

typedef struct {
    int id;
    int num_vertices;
    Vector vertices_local[MAX_VERTICES_PER_OBJECT];
    Vector vertices_world[MAX_VERTICES_PER_OBJECT];
    Vector vertices_camera[MAX_VERTICES_PER_OBJECT];

    int num_polys;
    Polygon polys[MAX_POLYS_PER_OBJECT];

    float radius;
    int state;
    Vector world_pos;
}Object;

Triangle triangle_create(const Vector* v1, const Vector* v2, const Vector* v3, int color);

void draw_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);
void draw_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);
void triangle_draw_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);

float compute_object_radius(Object* object);
char *PLG_Get_Line(char *string, int max_length, FILE *fp);
int PLG_Load_Object(Object* object, char *filename, float scale);
void remove_backfaces_and_shade(Object* object);
void draw_object_solid(Object* object, uint32_t* pixelmap);
void object_position(Object* object, int x, int y, int z);

void object_rotate_y(Object* object, float angle_rad);
void object_rotate_z(Object* object, float angle_rad);




#endif