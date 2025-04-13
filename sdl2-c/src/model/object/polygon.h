#ifndef POLYGON_H
#define POLYGON_H

#include "../math/vector.h"
#include "../math/matrix.h"
#include "../global.h"
#include "../../integration/display.h"
#include <stdint.h>
#include <stdio.h>

#define MAX_VERTICES_PER_OBJECT 17019
#define MAX_POINTS_PER_POLYGON 4
#define MAX_POLYS_PER_OBJECT 9720
#define MAX_POLYS_PER_FRAME 9720

// vertex_0 = top left
// vertex_1 = top right
// vertex_2 = bottom right
// vertex_3 = bottom left

typedef struct {
    int num_points;
    int vertex_list[MAX_POINTS_PER_POLYGON];
    Vector normal;

    int color;
    int shade[4];
    int two_sided;
    int visible;
    int active;
    int clipped;
    
}Polygon;

typedef struct {
    int num_points;     // number of vertices
    int color;       // color for each vertex of polygon
    int shade[4];       // the final shade of color lighting for each vertex of polygon
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

/* Object Transforms found in transform.c */

// Computes the maximum radius or sphere around object.
float compute_object_radius(Object* object);
// Rotates object along the y-axis.
void object_rotate_y(Object* object, float angle_rad);
// Rotates object along the z-axis.
void object_rotate_z(Object* object, float angle_rad);
// Transforms local coordinates to world coordinates by simply translating (adding) world pos 
// with local coordinates.
void object_local_to_world_transformation(Object* object);
// Convert the local coordinates into world and camera coordinates for shadin and projection. 
// Note the viewer is at (0,0,0) with angles 0,0,0 so the transformation is imply to add the 
// world position to each local vertex
void object_view_transformation(Object* object, Matrix* view_inverse);
// Sets the world position of an object.
static inline void object_position(Object* object, int x, int y, int z) {
    object->world_pos.x = x; object->world_pos.y = y; object->world_pos.z = z;
}

/* All clipping function found in clip.c */
void mirror_two_sided_polygons(Object *object);

// Determines if object is out of frame by comparing bounding sphere to z and then x,y frame.
// return 1 means object is out of frame and should be removed. 0 means it should not be removed.
int object_culling(Object* object, Matrix* view_inverse, int mode);
// Removes backfaces meaning that the method determines if polygons are invisible or clipped from
// the current viewpoint, and thus only draws relevant polygons. Relevant polygons of object are 
// also colored and shaded.
void remove_backfaces(Object* object, Vector* viewpoint, int mode);
// This fnuction clip an object in camera coordiantes against the 3D viewing
// volume. The function has 2 mode of operation. In CLIP_Z_MODE the 
// function performs only a simple z extend clip with the near and far clipping
// planes. In CLIP_XYZ_MODE the function performs a full 3D clip.
void clip_object_3D(Object* object, int mode);
// Function handles case for when polygon is only partly inside view frustum
// with the other part being in behind camera (z < CLIP_NEAR_Z). If this happens then
// approach is to determine intersection of polygon with the camera plane and clip the 
// polygon to these new points.
// If polygon is a quad then simply cut it up into 2 triangles and redo the process. Similar
// thing can happen if triangle only has 1 point outside of frame. 
void clip_polygon(facet *world_poly_storage, facet **world_polys, int *num_polys_frame);

/* All polygon list related function found in polygon.c */

// This function is used to generate the final plygon list that will be rendered. 
// Object by object the list is built up by converting into facets/polygons. 
void generate_poly_list(facet *world_poly_storage, facet **world_polys, int *num_polys_frame, Object* object);
// Draws all polygons in list. Similar to object_draw_solid.
void draw_poly_list_z(facet **world_polys, int *num_polys_frame, uint32_t* pixelmap, int *z_buffer);
// Resets polygon list by setting num_polys_frame to 0.
static inline void reset_poly_list(int *num_polys_frame) {
    *num_polys_frame = 0;
}

/* All draw triangle functions found in drawtriangle.c */

// Draws Triangles by determining float top or bottom triangle. 
// Same as draw_triangle_2D() except that this function incorporates a Z-buffer.
void draw_triangle_3D_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color[4], uint32_t* pixelmap, int *z_buffer, int mode);


// Extra shading function that breaks the triangle down using interpolation
// into even smaller areas. These areas then use a shading from 0-63 steps to 
// Achieve a finer look. Requires specific intensity.
void draw_tb_triangle_3d_gouraud(int x1, int y1, int z1, int i1,
                        int x2, int y2, int z2, int i2,
                        int x3, int y3, int z3, int i3,
                        uint32_t* pixelmap, int *z_buffer);

#endif