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
Polygon* object_create_triangle(const Vector* v1, const Vector* v2, const Vector* v3) {
    Polygon* triangle = malloc(sizeof(Polygon));
    triangle->num_verts = 3;
    triangle->center = vector_create(((v1->x + v2->x + v3->x) / 3), ((v1->y + v2->y + v3->y) / 3), ((v1->z + v2->z + v3->z) / 3));
    triangle->velocity = vector_create(0, 0, 0);

	const Vector** array_of_vertices = malloc(sizeof(Vector) * 3);
	array_of_vertices[0] = v1;
	array_of_vertices[1] = v2;
	array_of_vertices[2] = v3;
	triangle->vertice_matrix = vectors_as_matrix(array_of_vertices, 3);
	free(array_of_vertices);

	triangle->update = (object_update);
    return triangle;
}


/**
* Creates a object polygon structure that represents a Square.
* @param position_center vector position of square center (vertices will be generated 
*                        based on this point).
* @param width width of square.
* @param height of square.
*/
Polygon* object_create_square(Vector* position_center, float width, float height) {
	Polygon* square = malloc(sizeof(Polygon));
	square->num_verts = 4;
	square->center = position_center;
	square->velocity = vector_create(0, 0, 0);

	const Vector** array_of_vertices = malloc(sizeof(Vector) * 4);
	array_of_vertices[0] = vector_create((position_center->x - (width / 2)), (position_center->y + (height / 2)), position_center->z);
	array_of_vertices[1] = vector_create((position_center->x + (width / 2)), (position_center->y + (height / 2)), position_center->z);
	array_of_vertices[2] = vector_create((position_center->x + (width / 2)), (position_center->y - (height / 2)), position_center->z);
	array_of_vertices[3] = vector_create((position_center->x - (width / 2)), (position_center->y - (height / 2)), position_center->z);
    square->vertice_matrix = vectors_as_matrix(array_of_vertices, 4);

	square->update = (object_update);
    return square;
	
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

	if(io_is_key_down(io, SDL_SCANCODE_R)) {
		matrix_rotate(object->vertice_matrix, object->center, (M_PI / 32));
	}

	if(io_is_key_down(io, SDL_SCANCODE_O)) {
		matrix_scale(object->vertice_matrix, object->center, 1.1);
	}

	if(io_is_key_down(io, SDL_SCANCODE_P)) {
		matrix_scale(object->vertice_matrix, object->center, 0.9);
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

	Vector** vector_array = malloc(sizeof(Vector) * object->num_verts);
	for(int i = 0; i < object->num_verts; i++)
	{
		vector_array[i] = vector_from_matrix_row(object->vertice_matrix, i);
	}

	int p = 1;
	for(int i = 0; i < object->num_verts; i++) {
		display_draw_line(pixelmap, vector_array[i], vector_array[p], color);
		p = (p + 1) % object->num_verts;
	}

	display_draw_pixel(pixelmap, object->center->x, object->center->y, 0x00FFEEFF);
}