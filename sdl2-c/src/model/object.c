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
* Creates a polygon square based on 4 points (vectors).
* @param v1 top left point.
* @param v2 top right point.
* @param v3 bottom right point.
* @param v4 bottom left point.
*/
Polygon* object_create_square(const Vector* v1, const Vector* v2, const Vector* v3, const Vector* v4) {
	Polygon* square = malloc(sizeof(Polygon));
	square->num_verts = 4;
	square->center = vector_create(((v1->x + v2->x + v3->x + v4->x) / 4), ((v1->y + v2->y + v3->y + v4->y) / 4), ((v1->z + v2->z + v3->z + v4->z) / 4));;
	square->velocity = vector_create(0, 0, 0);

	const Vector** array_of_vertices = malloc(sizeof(Vector) * 4);
	array_of_vertices[0] = v1;
	array_of_vertices[1] = v2;
	array_of_vertices[2] = v3;
	array_of_vertices[3] = v4;
    square->vertice_matrix = vectors_as_matrix(array_of_vertices, 4);
	free(array_of_vertices);

	square->update = (object_update);
    return square;
	
}

/**
* Creates a 3D cube consisting of 6 squares. 
* @param position_center represents the singular center point of the cube.
* @param radius represent distance from center point to each face (i.e radius even though its not a circle).
* List of faces is represented as follows:
* [0] = front.
* [1] = right.
* [2] = back.
* [3] = left.
* [4] = top.
* [5] = bottom.
*/
Polygon3D* object_create_cube(Vector* position_center, float radius) {
	Polygon3D* cube = malloc(sizeof(Polygon3D));
	cube->center = position_center;
	cube->radius = radius;
	cube->velocity = vector_create(0, 0, 0);
	cube->num_faces = 6;
	cube->distance = 20;
	cube->face_list = malloc(sizeof(Polygon) * cube->num_faces);
	cube->face_list[0] = object_create_square(
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y + radius, position_center-> z - radius), 
											  vector_create(position_center->x + radius, position_center->y - radius, position_center-> z - radius), 
											  vector_create(position_center->x - radius, position_center->y - radius, position_center-> z - radius));

	cube->face_list[1] = object_create_square(
											  vector_create(position_center->x + radius, position_center->y + radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y + radius, position_center-> z + radius),
											  vector_create(position_center->x + radius, position_center->y - radius, position_center-> z + radius),
											  vector_create(position_center->x + radius, position_center->y - radius, position_center-> z - radius));

	cube->face_list[2] = object_create_square(
											  vector_create(position_center->x + radius, position_center->y + radius, position_center-> z + radius),
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z + radius),
											  vector_create(position_center->x - radius, position_center->y - radius, position_center-> z + radius),
											  vector_create(position_center->x + radius, position_center->y - radius, position_center-> z + radius));

	cube->face_list[3] = object_create_square(
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z + radius),
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z - radius),
											  vector_create(position_center->x - radius, position_center->y - radius, position_center-> z - radius),
											  vector_create(position_center->x - radius, position_center->y - radius, position_center-> z + radius));

	cube->face_list[4] = object_create_square(
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y + radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y + radius, position_center-> z + radius),
											  vector_create(position_center->x - radius, position_center->y + radius, position_center->z + radius));

	cube->face_list[5] = object_create_square(
											  vector_create(position_center->x - radius, position_center->y - radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y - radius, position_center->z - radius), 
											  vector_create(position_center->x + radius, position_center->y - radius, position_center-> z + radius),
											  vector_create(position_center->x - radius, position_center->y - radius, position_center->z + radius));

	return cube;										

}


/**
* Handles object related IO input (Object controls).
* Object specific direction vector set to OBJECT_SPEED.
* This velocity then needs to be set back to 0 in update.
*/
void object_io(IO* io, Polygon3D* object) {
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
	if(io_is_key_down(io, SDL_SCANCODE_I)) {
		object->velocity->z = OBJECT_SPEED;
	}
	if(io_is_key_down(io, SDL_SCANCODE_O)) {
		object->velocity->z = -OBJECT_SPEED;
	}

	/*
	if(io_is_key_down(io, SDL_SCANCODE_R)) {
		matrix_rotate(object->vertice_matrix, object->center, (M_PI / 32));
	}

	if(io_is_key_down(io, SDL_SCANCODE_O)) {
		matrix_scale(object->vertice_matrix, object->center, 1.1);
	}

	if(io_is_key_down(io, SDL_SCANCODE_P)) {
		matrix_scale(object->vertice_matrix, object->center, 0.9);
	}
	*/
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
* Applies perspective on a 3D object the same as an object is scaled.
* That is, by transforming the matrix to the origin,
* peform scaling according to camera distance,
* transform back to original position.
* @param object3d object that perspective scaling is applied on.
* @param distance represents camera distance.
*/
void object_perspective_3d(Polygon3D* object3d, float distance) {
	Vector p_negate = *object3d->center;
	vector_negate(&p_negate);
	Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
	translate_to_origin->matrix[3][2] = 0;
	for(int i = 0; i < object3d->num_faces; i++) {
		*object3d->face_list[i]->vertice_matrix = (*matrix_mul(object3d->face_list[i]->vertice_matrix, translate_to_origin));
	}

	for(int i = 0; i < object3d->num_faces; i++) {
		matrix_perspective_transformation(object3d->face_list[i]->vertice_matrix, distance);
		//possibly perspective on face centerpoints aswell.
	}

	Matrix* translate_back = matrix_create_translation_matrix(object3d->center);
	translate_back->matrix[3][2] = 0;
	for(int i = 0; i < object3d->num_faces; i++) {
		*object3d->face_list[i]->vertice_matrix = (*matrix_mul(object3d->face_list[i]->vertice_matrix, translate_back));
	}
}


/**
* Updates 3D object by performing individual updates for all faces.
* @param object3d 3D object to be update.
* @param distance camera distance.
*/
void object_update_3d(Polygon3D* object3d) {
	Matrix* translate = matrix_create_translation_matrix(object3d->velocity);
	for(int i = 0; i < object3d->num_faces; i++)
	{
		*object3d->face_list[i]->vertice_matrix = (*matrix_mul(object3d->face_list[i]->vertice_matrix, translate));
		*object3d->face_list[i]->center = (*vector_add(object3d->center, object3d->velocity));
	}
	*object3d->center = (*vector_add(object3d->center, object3d->velocity));
	object3d->velocity->x = 0;
	object3d->velocity->y = 0;
	object3d->velocity->z = 0;
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


/**
* Draws a 3d object by drawing all faces of that object.
*/
void object_draw_3d(uint32_t* pixelmap, Polygon3D* object, float distance, uint32_t color) {

	Polygon3D* copy = object_create_cube(object->center, object->radius);
	object_perspective_3d(copy, distance);

	for(int i = 0; i < object->num_faces; i++)
	{	
		uint32_t colorfixed = 0x0000FFFF;
		object_draw(pixelmap, copy->face_list[i], colorfixed);
		colorfixed += 0x000FFFFF;
		display_draw_pixel(pixelmap, copy->face_list[i]->center->x, copy->face_list[i]->center->y, 0x00FFEEFF);
	}

	display_draw_pixel(pixelmap, object->center->x, object->center->y, 0x003F0EFF);
	free(copy);
}