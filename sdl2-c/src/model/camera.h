#include "vector.h"

#ifndef CAMERA_H
#define CAMERA_H

/**
* Camera consist of player position, direction and a camera plane (perpendicular to direction).
*/
typedef struct {
    Vector* position;
    Vector* direction;
    Vector* camera_plane;
    float speed;
}Camera;

#endif