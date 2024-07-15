
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



#endif