
#ifndef DISPLAY_H
#define DISPLAY_H

#include "../model/global.h"
#include "../model/math/vector.h"
#include <stdint.h>


// Draws pixel on screen basd on coordinates x and y.
// If coordinates land outside of window proportions then function
// simply returns without making any changes.
static inline void display_draw_pixel(uint32_t* pixelmap, int x, int y, uint32_t color) {
    if((y > 0 || y < WINDOW_HEIGHT) || (x > 0 || x < WINDOW_WIDTH)) {
        pixelmap[PIXEL(x,y)] = color;
    }
    return;
}

void display_draw_line(uint32_t* pixelmap, const Vector* v1, const Vector* v2, uint32_t color);

#endif