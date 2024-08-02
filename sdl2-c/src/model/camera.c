#include "camera.h"
#include "matrix.h"
#include "vector.h"
#include <stdlib.h>



Camera* camera_init(Vector* position) {
    Camera* camera = malloc(sizeof(Camera));

    camera->position = position;
    camera->direction = vector_create(0, 0, 1);
    camera->up = vector_create(0, 1, 0);
    camera->camera_plane = vector_create(1,0,0);

    camera->pointAt = matrix_point_at(camera->position, vector_create(0,0,1), camera->up);
    camera->lookAt = matrix_quick_lookat_inverse(camera->pointAt);

    camera->fYaw = 0.0f;

    return camera;
}


/**
* Update of Camera logistics.
*
* 1. fYaw rotation.
*
*/
void camera_update(Camera* camera) {

    //constant camera vectors.
    camera->up = vector_create(0,1,0);
    Matrix* rotation_y = matrix_create_rotation_matrix_y(camera->fYaw);

    // calculate pointAt matrix and find inverse lookAt.
    camera->pointAt = matrix_point_at(camera->position, vector_create(0, 0, 1), camera->up);
    camera->lookAt = matrix_quick_lookat_inverse(camera->pointAt);

    // apply rotation onto lookAt matrix.
    camera->direction = vector_matrix_mul(vector_create(0,0,1), rotation_y);
    camera->direction->x = -camera->direction->x;
    camera->lookAt = matrix_mul(camera->lookAt, rotation_y);
    
    
    // camera_plane perpendicular to direction
    Matrix* planerot = matrix_create_rotation_matrix_y((M_PI / 2));
    camera->camera_plane = vector_matrix_mul(camera->direction, planerot);
}