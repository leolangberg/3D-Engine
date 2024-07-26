#include "vector.h"
#include "matrix.h"

#ifndef CAMERA_H
#define CAMERA_H

/**
* Camera consist of player position, direction and a camera plane (perpendicular to direction).
*/
typedef struct {
    Vector* position;
    Vector* direction;
    Vector* camera_plane;

    Vector* up;
    Vector* target;

    Matrix* pointAt;
    Matrix* lookAt;
    float fYaw;

    struct {
        Vector* world_origin;
        Vector* view_origin;
        Vector* screen_origin;
    }origin;
}Camera;

Camera* camera_init(Vector* position);

void camera_update(Camera* camera);

#endif