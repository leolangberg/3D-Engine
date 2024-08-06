#ifndef SECTOR_H
#define SECTOR_H

#include "polygon.h"


typedef struct {
    int id;
    int num_walls;
    int floor;
    int ceiling;

    Object wall_list[8];

}Sector;

#endif