#include "display.h"

/**
* Bresenhams Algorithm for vector rasterization.
*
* Decides direction based on slope then proceed with the actual algorithm
* that basically walks the line in steps and swapping x, y values depending
* on if an error margin has passed.
*
* Plots line from v1 to v2.
* @param v1 start point.
* @param v2 end point.
* 
* Lines could be cut in middle and rendered twice as fast.
*/
void bresenhams_algorithm(uint32_t* pixelmap, const Vector* v1, const Vector* v2, uint32_t color) {

    /**
    * dx: difference in x's
    * dy: difference in y's 
    * dx2: dx,dy * 2
    * dy2:
    * x_inc: amount in pixel space to move during drawing
    * y_inc: amount in pixel space to move during drawing
    * error: the discriminant i.e. error i.e. decision variable
    */
    int dx, dy, dx2, dy2, x_inc, y_inc, error;

    /**
    * Draws first pixel on screen.
    */
    int cur_x = v1->x;
    int cur_y = v1->y;
    display_draw_pixel(pixelmap, cur_x, cur_y, color);

    /**
    * Compute horizontal and vertical deltas.
    */
    dx = v2->x - v1->x;
    dy = v2->y - v1->y;

    /*
    * Test which direction the line is going in (slope ange).
    * Determines if line ins moving right or left.
    * Depending on direction dx is reverse (absolute value). 
    */
    if(dx >= 0) {
        x_inc = 1;
    } else {
        x_inc = -1;
        dx = (-dx);
    }

    if(dy >= 0) {
        y_inc = 1;
    } else {
        y_inc = (-1);
        dy = (-dy);
    }

    dx2 = dx * 2;
    dy2 = dy * 2;

    /**
    * Decides direction based on slope then proceed with the actual algorithm
    * that basically walks the line in steps and swapping x, y values depending
    * on if an error margin has passed.
    */
    if(dx > dy) {
        error = dy2 - dx;

        for(int i = 0; i < dx; i++)
        {
            if(error >= 0) {
                error -= dx2;
                cur_y += y_inc;
                //display_draw_pixel(pixelmap, cur_x, cur_y, color);
            } 
            error += dy2;
            cur_x += x_inc;
            display_draw_pixel(pixelmap, cur_x, cur_y, color);
        }
    }
    else {
        error = dx2 - dy;

        for(int i = 0; i < dy; i++)
        {
            if(error >= 0) {
                error -= dy2;
                cur_x += x_inc;
                //display_draw_pixel(pixelmap, cur_x, cur_y, color);
            }
            error += dx2;
            cur_y += y_inc;
            display_draw_pixel(pixelmap, cur_x, cur_y, color);
        }
    }
}

/* Display a line between 2 vectors (or points) by calling bresenhams algorithm. */
void display_draw_line(uint32_t* pixelmap, const Vector* v1, const Vector* v2, uint32_t color) {
    bresenhams_algorithm(pixelmap, v1, v2, color);
}