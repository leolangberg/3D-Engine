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
    camera->target = vector_add(camera->position, camera->direction);

    camera->pointAt = matrix_point_at(camera->position, camera->target, camera->up);
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

    camera->up = vector_create(0,1,0);
    camera->target = vector_create(0,0,1);

    Matrix* rotation_y = matrix_create_rotation_matrix_y(camera->fYaw);
    camera->direction = vector_matrix_mul(camera->target, rotation_y);
    
    Matrix* planerot = matrix_create_rotation_matrix_y((M_PI / 2));
    camera->camera_plane = vector_matrix_mul(camera->direction, planerot);
   
    camera->target = vector_add(camera->position, camera->direction);
    camera->pointAt = matrix_point_at(camera->position, camera->target, camera->up);
    camera->lookAt = matrix_quick_lookat_inverse(camera->pointAt);


}