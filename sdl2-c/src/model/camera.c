#include "camera.h"
#include "math/matrix.h"
#include "math/vector.h"
#include <stdlib.h>

// Initializes camera by finding position, direction, up 
// and plane vectors, with these the pointAt and lookAt
// matrices can be created.
Camera* camera_init(const Vector* position) {
    Camera* camera = malloc(sizeof(Camera));   
    Vector locked_z = vector_create(0,0,1);
    camera->position     = vector_copy(position);
    camera->direction    = vector_create(0, 0, 1);
    camera->up           = vector_create(0, 1, 0);
    camera->camera_plane = vector_create(1, 0, 0);

    camera->pointAt = matrix_point_at(&camera->position, &locked_z, &camera->up);
    camera->lookAt  = matrix_quick_lookat_inverse(&camera->pointAt);
    camera->fYaw  = 0.0f;
    camera->pitch = 0.0f;
    return camera;
}

// Update of Camera logistics.
// To determine pointAt matrix 2 constant vectors (0,1,0) & (0,0,1) is used.
// Only after this is the rotation applied to the world lookAt matrix.
void camera_update(Camera* camera) {
    Matrix rotation_y = matrix_create_rotation_matrix_y(camera->fYaw);
    Matrix rotation_x = matrix_create_rotation_matrix_x(camera->pitch);
    Vector locked_z = vector_create(0, 0, 1);
    //camera->up      = vector_create(0, 1, 0);

    // calculate pointAt matrix and find inverse lookAt.
    camera->pointAt = matrix_point_at(&camera->position, &locked_z, &camera->up);
    camera->lookAt  = matrix_quick_lookat_inverse(&camera->pointAt);

    // apply rotation onto lookAt matrix.
    camera->direction = vector_matrix_mul(&locked_z, &rotation_y);
    camera->lookAt = matrix_mul(&camera->lookAt, &rotation_y);
    camera->lookAt = matrix_mul(&camera->lookAt, &rotation_x);
    camera->direction.x = -camera->direction.x;

    // camera_plane perpendicular to direction
    Matrix planerot = matrix_create_rotation_matrix_y((M_PI / 2));
    camera->camera_plane = vector_matrix_mul(&camera->direction, &planerot);
}