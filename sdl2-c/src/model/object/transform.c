#include <stdio.h>
#include "polygon.h"

// Computes the maximum radius or sphere around object.
float compute_object_radius(Object* object) {
    float new_radius, x,y,z;    // used in average radious calculation of object
    object->radius = 0;
    for(int index = 0; index < object->num_vertices; index++) {
        x = object->vertices_local[index].x;
        y = object->vertices_local[index].y;
        z = object->vertices_local[index].z;
        // compute distance to point and if this radius is bigger than
        // last then change to bigger radius.
        new_radius = (float) d_sqrt((x*x) + (y*y) + (z*z));
        if(new_radius > object->radius) {
            object->radius = new_radius;
        }
    }
    return object->radius;
}

// Rotates object along the y-axis via matrix multiplication.
void object_rotate_y(Object* object, float angle_rad) {
    Matrix m, y = matrix_create_rotation_matrix_y(angle_rad);
    for(int index = 0; index < object->num_vertices; index++) {
        m = vector_as_matrix(&object->vertices_local[index]);
        m = matrix_mul(&m, &y);
        object->vertices_local[index].x = m.matrix[0][0];
        object->vertices_local[index].y = m.matrix[0][1];
        object->vertices_local[index].z = m.matrix[0][2];
    }

}

// Rotates object along the z-axis via matrix multiplication.
void object_rotate_z(Object* object, float angle_rad) {
    Matrix m, z = matrix_create_rotation_matrix_z(angle_rad);
    for(int index = 0; index < object->num_vertices; index++) {
        m = vector_as_matrix(&object->vertices_local[index]);
        m = matrix_mul(&m, &z);
        object->vertices_local[index].x = m.matrix[0][0];
        object->vertices_local[index].y = m.matrix[0][1];
        object->vertices_local[index].z = m.matrix[0][2];
    }

}

// Transforms local coordinates to world coordinates by simply translating (adding) world pos 
// with local coordinates.
void object_local_to_world_transformation(Object* object) {
    for(int index = 0; index < object->num_vertices; index++) {
        object->vertices_world[index].x = object->vertices_local[index].x + object->world_pos.x;
        object->vertices_world[index].y = object->vertices_local[index].y + object->world_pos.y;
        object->vertices_world[index].z = object->vertices_local[index].z + object->world_pos.z;   
    }
}

// Convert the local coordinates into world and camera coordinates for shading
// and projection. Note the viewer is at (0,0,0) with angles 0,0,0 so the 
// transformation is imply to add the world position to each  local vertex
void object_view_transformation(Object *object, Matrix *view_inverse) {  
        for(int index = 0; index < object->num_vertices; index++) {
            object->vertices_camera[index] = vector_matrix_mul(&object->vertices_world[index], view_inverse);
        }
}