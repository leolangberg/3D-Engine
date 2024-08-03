#include "math/vector.h"
#include "math/matrix.h"

#ifndef CAMERA_H
#define CAMERA_H

/**
* Camera consist of player position, direction and a camera plane (perpendicular to direction).
*/
typedef struct {
    Vector* position;       // position of camera (viewpoint)
    Vector* direction;      // direction camera is facing
    Vector* camera_plane;   // plane perpendicular to direction
    Vector* up;             // constant up vector (0,1,0)

    Matrix* pointAt;        // point At matrix
    Matrix* lookAt;         // inverse look At matrix
    float fYaw;             // rotation on Y-axis
    float pitch;            // rotation on X-axis
}Camera;

Camera* camera_init(Vector* position);

void camera_update(Camera* camera);

#endif