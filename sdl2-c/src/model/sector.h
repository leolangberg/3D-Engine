

#ifndef SECTOR_H
#define SECTOR_H

#include "vector.h"


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
    Vector* v1;
    Vector* v2;
    Vector* h;
}Wall;




#endif