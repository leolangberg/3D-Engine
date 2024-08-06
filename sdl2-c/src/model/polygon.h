#ifndef POLYGON_H
#define POLYGON_H

#include "math/vector.h"
#include "math/matrix.h"
#include "global.h"
#include <stdint.h>
#include <stdio.h>


#define MAX_VERTICES_PER_OBJECT 24
#define MAX_POINTS_PER_POLYGON 4
#define MAX_POLYS_PER_OBJECT 6

#define MAX_POLYS_PER_FRAME 128



typedef struct {
    int num_points;
    int vertex_list[MAX_POINTS_PER_POLYGON];
    Vector normal;

    int color;
    int shade;
    int two_sided;
    int visible;
    int active;
    int clipped;
    
}Polygon;

typedef struct {
    int num_points;     // number of vertices
    int color;          // color of polygon
    int shade;          // the final shade of color lighting
    int two_sided;      // is the facet two sided
    int visible;        // is the facet transparent
    int clipped;        // has this poly been clipped
    int active;         // used to turn faces on and off

    Vector vertex_list[MAX_POINTS_PER_POLYGON]; // the points that make up the polygon facet
    Vector normal;
}facet, *facet_ptr;

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




extern Vector light_source;
extern float  ambient_light;
extern int num_polys_frame;

facet  world_poly_storage[MAX_POLYS_PER_FRAME];
facet* world_polys[MAX_POLYS_PER_FRAME];

int z_buffer[WINDOW_WIDTH * WINDOW_HEIGHT]; // does not require 2 separate 64k byte arrays because its 2024.

/**
* Draws Triangle on pixel screen.
*
* Algorithm determines whether triangle drawn is one of the following cases:
* Flat top, Flat bottom or General. In case of general the triangle is sorted and 
* split dividing the triangle into 2 now distint flat top or bottom triangles.
*/
void triangle_draw_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap);

/**
* Removes backfaces meaning that the method determines if polygons are invisible or clipped from
* the current viewpoint, and thus only draws relevant polygons. 
* Relevant polygons of object are also colored and shaded correctly based on direction 
* to light source.
*/
void remove_backfaces_and_shade(Object* object, Vector* viewpoint, int mode);

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

/**
* Transforms local coordinates to world coordinates by simply translating (adding) world pos with
* local coordinates.
*/
void object_local_to_world_transformation(Object* object);

/**
* Convert the local coordinates into world and camera coordinates for shading
* and projection. Note the viewer is at (0,0,0) with angles 0,0,0
* so the transformation is imply to add the world position to each 
* local vertex
*/
void object_view_transformation(Object* object, Matrix* view_inverse);

/**
* Determines if object is out of frame by comparing bounding sphere to z and then x,y frame.
* @return 1 means object is out of frame and should be removed. 0 means it should not be removed.
*/
int object_culling(Object* object, Matrix* view_inverse, int mode);

void clip_polygon_near_z();

void clip_polygon();
/**
* This fnuction clip an object in camera coordiantes against the 3D viewing
* volume. The function has 2 mode of operation. In CLIP_Z_MODE the 
* function performs only a simple z extend clip with the near and far clipping
* planes. In CLIP_XYZ_MODE the function performs a full 3D clip.
*/
void clip_object_3D(Object* object, int mode);

/**
* This function is used to generate the final plygon list that will be
* rendered. Object by object the list is built up.
* To reset list call (NULL, RESET_POLY_LIST)
*/
void generate_poly_list(Object* object, int mode);

/**
* Calls qsort() function to perform sort of polygon list based on
* descending z-value.
*/
void sort_polygon_list(void);

/*
* Draws all polygons in list. Similar to object_draw_solid.
*/
void draw_poly_list(uint32_t *pixelmap);

/**
* Fills z_buffer with constant value that needs to be 
* much larger than any normal z value.
*/
void fill_z_buffer(int value);

/**
* Draws Triangles by determining float top or bottom triangle. 
* Same as draw_triangle_2D() except that this function incorporates 
* the Z-buffer.
*/
void draw_triangle_3D_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color, uint32_t* pixelmap);

/**
* Same as draw_poly_list() except that this function also
* incorporates the Z-buffer.
*/
void draw_poly_list_z(uint32_t* pixelmap);

/**
* Extra shading function that breaks the triangle down using interpolation
* into even smaller areas. These areas then use a shading from 0-63 steps to 
* Achieve a finer look.
* DOES NOT QUITE WORK AS OF NOW.
*/
void draw_triangle_2d_gouraud(int x1, int y1, int x2, int y2, int x3, int y3, int color, int intensity_1, int intensity_2, int intensity_3, uint32_t *pixelmap);



#endif