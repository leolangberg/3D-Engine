#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "matrix.h"
#include "global.h"
#include <stdint.h>
#include <stdio.h>


#define MAX_VERTICES_PER_OBJECT 24
#define MAX_POINTS_PER_POLYGON 4
#define MAX_POLYS_PER_OBJECT 6

#define poly_clip_max_x WINDOW_WIDTH - 1
#define poly_clip_max_y WINDOW_HEIGHT - 1
#define poly_clip_min_x 0
#define poly_clip_min_y 0


#define FLAT_SHADING 1

#define SHADE_GREY 31           // hex value = 1F
#define SHADE_GREEN 111         // hex value = 6F
#define SHADE_BLUE 159          // hex value = 9F
#define SHADE_RED 47            // hex value = 2F
#define SHADE_YELLOW 79         // hex value = 4F
#define SHADE_BROWN 223         // hex value = DF
#define SHADE_LIGHT_BROWN 207   // hex value = CF
#define SHADE_PURPLE 175        // hex value = AF
#define SHADE_CYAN 127          // hex value = 7F
#define SHADE_LAVENDER 191      // hex value = BF





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

/**
* Draws Triangle on pixel screen.
*
* Algorithm determines whether triangle drawn is one of the following cases:
* Flat top, Flat bottom or General. In case of general the triangle is sorted and 
* split dividing the triangle into 2 now distint flat top or bottom triangles.
*/
void triangle_draw_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);

/**
* Loads PLG files by reading from the text file and declaring variables accordingly.
* Also has the option to scale the object as it is being constructed.
*/
int PLG_Load_Object(Object* object, char *filename, float scale);

/**
* Removes backfaces meaning that the method determines if polygons are invisible or clipped from
* the current viewpoint, and thus only draws relevant polygons. 
* Relevant polygons of object are also colored and shaded correctly based on direction 
* to light source.
*/
void remove_backfaces_and_shade(Object* object, Vector* viewpoint);

/**
* All relevant and visible sides of the object are converted into screen space
* and then drawn via triangle_draw_2d() method.
*/
void draw_object_solid(Object* object, uint32_t* pixelmap);

/**
* Computes the maximum radius or sphere around object.
*/
float compute_object_radius(Object* object);

/**
* Sets the world position of an object.
*/
void object_position(Object* object, int x, int y, int z);

/**
* Rotates object along the y-axis.
*/
void object_rotate_y(Object* object, float angle_rad);

/**
* Rotates object along the z-axis.
*/
void object_rotate_z(Object* object, float angle_rad);




#endif