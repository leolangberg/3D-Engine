
#ifndef DISPLAY_H
#define DISPLAY_H

#include "../model/vector.h"
#include <stdint.h>

/**
* Draws pixel on screen basd on coordinates x and y.
* If coordinates land outside of window proportions then function
* simply returns without making any changes.
*/
void display_draw_pixel(uint32_t* pixelmap, int x, int y, uint32_t color);

/**
* Display a line between 2 vectors (or points) by calling bresenhams algorithm.
*/
void display_draw_line(uint32_t* pixelmap, const Vector* v1, const Vector* v2, uint32_t color);

/**
* Paints a 2D rectangle of sector described. 
*/
void display_sector_2d(uint32_t* pixelmap, int x0, int y0, int width, int height, uint32_t color);

/**
* Paints map onto pixelscreen.
*
* Iterates through each sector in mapdata and if this sectors == 1, then it assumed to be a wall.
* This wall is thus painted white and xpos is moved onto next sector.
* Once xpos = window_width then reset to 0 and decrease y by 1 sector, thus scanning whole area.
*/
void display_map(int* mapdata, uint32_t* pixelmap);

#endif