#ifndef WALL_H
#define WALL_H

#include "vector.h"

typedef struct Wall{
    int id;                 // used for debugging
    int color;              // color of wall
    Vector wall_world[4];   // the points that make up the wall
    Vector wall_camera[4];  // the final camera coordinates of the wall
    Vector normal;          // the outward noraml to the wall used during
                            // creation of BSP only, after that it becomes invalid

    struct Wall *link;      // pointer to next wall
    struct Wall *front;     // pointer to walls in front
    struct Wall *back;      // pointer to walls behind

}Wall;


#endif