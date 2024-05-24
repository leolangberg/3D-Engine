#include <stdint.h>
#include <stdlib.h>
#include "object.h"
#include "SDL2/SDL_scancode.h"
#include "matrix.h"
#include "vector.h"
#include "../integration/display.h"

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
    m->matrix[3][0] = 0;
    m->matrix[3][1] = 0;
    m->matrix[3][2] = 0;
    m->matrix[3][3] = 0; 

    triangle->vertice_matrix = m;
    return triangle;
}

/**
* Handles object related IO input (Object controls).
* Object specific direction vector set to OBJECT_SPEED.
* This velocity then needs to be set back to 0 in update.
*/
void object_io(IO* io, Polygon* object) {
    if(io_is_key_down(io, SDL_SCANCODE_W)) {
       (object->velocity->y) = OBJECT_SPEED;
    }
    if(io_is_key_down(io, SDL_SCANCODE_S)) {
        object->velocity->y = -OBJECT_SPEED;
    }
    if(io_is_key_down(io, SDL_SCANCODE_D)) {
        object->velocity->x = OBJECT_SPEED;
    }
    if(io_is_key_down(io, SDL_SCANCODE_A)) {
        object->velocity->x = -OBJECT_SPEED;
    }
}

/**
* Creates a translation matrix out of the velocity vector and then 
* performs matrix multiplication with current vertices to get new position.
* Also performs vector addition to update object center.
* Resets velocity.
*/
void object_update(Polygon* object) {
    Matrix* translate = matrix_create_translation_matrix(object->velocity);
    *object->vertice_matrix = (*matrix_mul(object->vertice_matrix, translate));
    *object->center = (*vector_add(object->center, object->velocity));
	object->velocity->x = 0;
	object->velocity->y = 0;
	object->velocity->z = 0;
}

/**
* Draws object onto the pixelmap screen by retrieving all vertices
* and then drawing out the lines between them.
*/
void object_draw(uint32_t* pixelmap, Polygon* object, uint32_t color) {
    Vector* v1 = vector_from_matrix_row(object->vertice_matrix, 0);
    Vector* v2 = vector_from_matrix_row(object->vertice_matrix, 1);
    Vector* v3 = vector_from_matrix_row(object->vertice_matrix, 2);

    display_draw_line(pixelmap, v1, v2, color);
    display_draw_line(pixelmap, v2, v3, color);
    display_draw_line(pixelmap, v3, v1, color);
}