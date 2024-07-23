#include "display.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

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

/**
* Draws pixel on screen basd on coordinates x and y.
* If coordinates land outside of window proportions then function
* simply returns without making any changes.
*/
void display_draw_pixel(uint32_t* pixelmap, int x, int y, uint32_t color) {
    if(y < 0 || y >= WINDOW_HEIGHT) {
        //printf("pixel not drawn: (%d,%d)\n", x, y);
        return;
    }
    if(x < 0 || x >= WINDOW_WIDTH) {
        //printf("pixel not drawn: (%d,%d)\n", x, y);
        return;
    }
    pixelmap[(y * WINDOW_WIDTH) + x] = color;
    //printf("DRAW PIXEL: (%d,%d) pxmap: %d\n", x, y, pixelmap[(y * 384) + x]);
}

/**
* Display a line between 2 vectors (or points) by calling bresenhams algorithm.
*/
void display_draw_line(uint32_t* pixelmap, const Vector* v1, const Vector* v2, uint32_t color) {
    bresenhams_algorithm(pixelmap, v1, v2, color);
}



/**
* Raycasting Algorithm
* https://lodev.org/cgtutor/raycasting.html
*
* For each x in width (of pixelmap) a distance to objects infront can be determined (thus Renders vertical).
* To begin with the rays direction is determined by adding the position together with the player direction and 
* through this information the delta (steps) for each iteration can be calculated which corresponds to movement on the worldmap grid.
* 
* Iteration in delta steps keeps increasing as long as a wall is not hit (DAA-Algorithm). Once a wall has finally been hit the 
* variable perpWallDist is used to draw parallell lines along the direction vector for the entire Field of View (To avoid Euclidean length and fisheye).
*
* Finally the wall height can be determined by dividing pixelmap height by perpWallDist. The start and end points of this vertical line is then 
* calculated and everything inbetween rendered with corresponding color.
*
* Side 0 = x-direction and side 1 = y-direction.
* 
*/
void raycasting_algorithm(Camera* camera, int worldmap[24][24], uint32_t* pixelmap, int texture[8][64 * 63 + 63]) {
   
    int w = WINDOW_WIDTH;
    int h = WINDOW_HEIGHT;

    for(int x = 0; x < w; x++)
    {
        float cameraX = 2.0 * x / w - 1;
        float rayDirX = camera->direction->x + camera->camera_plane->x * cameraX;
        float rayDirY = camera->direction->y + camera->camera_plane->y * cameraX;

        //which box of the map were in.
        int mapX = camera->position->x;
        int mapY = camera->position->y;

        //length of ray from current position to next x or y-side.
        float sideDistX;
        float sideDistY;

        /**
        * length of ray from one x or y-side to next x or y-side.
        * deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
        * deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
        */
        float deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX); //? in case division by 0.
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        float perpWallDist;

        //what direction to step in x or y. (either +1 or -1)
        int stepX;
        int stepY;

        int hit = 0;    //was there a wall hit?
        int side;   //was a North-South or a East-West wall hit?

            //calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (camera->position->x - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - camera->position->x) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (camera->position->y - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - camera->position->y) * deltaDistY;
        }

        //perform DDA-Algorithm.
        while (hit == 0)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            //Check if ray has hit a wall
            if (worldmap[mapX][mapY] > 0) {
                hit = 1;
            }
        }

        //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
        if(side == 0) { 
            perpWallDist = (sideDistX - deltaDistX); 
        } else {
            perpWallDist = (sideDistY - deltaDistY);
        }

        //Calculate height of line to draw on screen
        int lineHeight = (int)(h / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + h / 2;
        if(drawStart < 0) {
            drawStart = 0;
        }
        int drawEnd = lineHeight / 2 + h / 2;
        if(drawEnd >= h) {
            drawEnd = h - 1;
        }

        /**
        * Addition for textured raycasters 
        */

        
        //if you go outside the map then this worldmap value and thus texture_number becomes screwed.
        int texture_number = worldmap[mapX][mapY] - 1;

        //calculate value of wallX
        float wallX;
        if(side == 0) {
            wallX = camera->position->y + perpWallDist * rayDirY;
        } else {
            wallX = camera->position->x + perpWallDist * rayDirX;
        }
        wallX -= floorf(wallX);

        //x coordinate on the texture
        int textureX = wallX * 64;
        if(side == 0 && rayDirX > 0) {
            textureX = 64 - textureX - 1;
        }
        if(side == 1 && rayDirY < 0) {
            textureX = 64 - textureX - 1;
        }

        //how much to increase the texture coordinate per screen pixel.
        float step = 64.0 / lineHeight;
        //starting texture coordinate
        float texture_pos = (drawStart - (float) h  / 2 + (float) lineHeight / 2) * step;
        for(int y = drawStart; y < drawEnd; y++) 
        {
            //Cast the texture coordinate to integer, and mask with TEXTUREHEIGHT (64) - 1 in case of overflow
            int textureY = (int) texture_pos & (64 - 1);
            texture_pos += step;
            uint32_t color = texture[texture_number][64 * textureY + textureX]; //segment fault 11.

            //make y-dir colors slight darker
            if(side == 1) {
                color = (color / 2);
            }
            //no point in having a buffer as pixels are already drawn one by one. 
            display_draw_pixel(pixelmap, x, y, color);
        }
    }
}