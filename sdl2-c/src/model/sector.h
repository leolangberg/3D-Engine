

#ifndef SECTOR_H
#define SECTOR_H

#include "vector.h"
#include "matrix.h"
#include <stdint.h>


/**
* sector: <id> <index> <num walls> <floor> <ceiling>
*/
typedef struct {
    int id;
    int index, nwalls;
    float zFloor, zCeil;
}Sector;

/**
* Wall defined by 2 points.
* (Height defiend in sector)
*/
typedef struct {
    Matrix wall;
    uint32_t color;
    Vector n;
}Wall;

Wall wall_create(const Vector* v1, const Vector* v2, const Vector* v3, const Vector* v4, uint32_t color);


#endif