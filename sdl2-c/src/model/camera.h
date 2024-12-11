#include "math/vector.h"
#include "math/matrix.h"
#include <math.h>

#ifndef CAMERA_H
#define CAMERA_H

// Camera consist of player position, direction 
// and a camera plane (perpendicular to direction).
typedef struct {
    Vector position,       // position of camera (viewpoint
           direction,      // direction camera is facing
           camera_plane,   // plane perpendicular to direction
           up;             // constant up vector (0,1,0)
    Matrix pointAt,        // point At matrix
           lookAt;         // inverse look At matrix
    float fYaw;             // rotation on Y-axis
    float pitch;            // rotation on X-axis
}Camera;

// Initializes camera by finding position, direction, up 
// and plane vectors, with these the pointAt and lookAt
// matrices can be created.
Camera* camera_init(const Vector* position);

// Update of Camera logistics.
// To determine pointAt matrix 2 constant vectors (0,1,0) & (0,0,1) is used.
// Only after this is the rotation applied to the world lookAt matrix.
void camera_update(Camera* camera);

#endif