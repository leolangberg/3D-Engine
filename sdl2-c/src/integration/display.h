
#ifndef DISPLAY_H
#define DISPLAY_H

#include "../model/math/vector.h"
#include "../model/camera.h"
#include "../model/global.h"
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
void display_draw_line(uint32_t* pixelmap, Vector* v1, Vector* v2, uint32_t color);

/**
* Raycasting Algorithm
*
* For each x in width (of pixelmap) a distance to objects infront can be determined (thus Renders vertical lines).
* To begin with the rays direction is determined by adding the position together with the player direction and 
* through this information the delta (steps) for each iteration can be calculated which corresponds to movement on the worldmap grid.
* 
* Iteration in delta steps keeps increasing as long as a wall is not hit (DAA-Algorithm). Once a wall has finally been hit the 
* variable perpWallDist is used to draw parallell lines along the direction vector for the entire Field of View (To avoid Euclidean length and fisheye).
*
* Finally the wall height can be determined by dividing pixelmap height by perpWallDist. The start and end points of this vertical line is then 
* calculated and everything inbetween rendered with corresponding color.
* 
*/
void raycasting_algorithm(Camera* camera, int worldmap[24][24], uint32_t* pixelmap);


/**
* Cohen-Sutherland Algorithm for clipping 2D lines.
* 
* Screen is divided into sectors depending on x and y values one can determine which sector the point is in.
* The CLIP_CODEs serve a purpose of being addable on top of each other so that N & E = NE.
* Once the sector has been determined for each point the new intersection points are calculated given.
*/
int clip_line(Vector* v1, Vector* v2);

#endif