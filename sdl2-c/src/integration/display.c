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
}

/**
* Display a line between 2 vectors (or points) by calling bresenhams algorithm.
*/
void display_draw_line(uint32_t* pixelmap, Vector* v1, Vector* v2, uint32_t color) {
    
    if(clip_line(v1, v2)) {  bresenhams_algorithm(pixelmap, v1, v2, color); }
    
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
void raycasting_algorithm(Camera* camera, int worldmap[24][24], uint32_t* pixelmap) {
   
    int w = WINDOW_WIDTH;
    int h = WINDOW_HEIGHT;

    for(int x = 0; x < w; x++)
    {
        float cameraX = 2.0 * x / w - 1;
        float rayDirX = camera->direction->x + camera->camera_plane->x * cameraX;
        float rayDirY = camera->direction->z + camera->camera_plane->z * cameraX;

        //which box of the map were in.
        int mapX = camera->position->x;
        int mapY = camera->position->z;

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
            sideDistY = (camera->position->z - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - camera->position->z) * deltaDistY;
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


        uint32_t color;
        switch(worldmap[mapX][mapY])
        {
            case 1:  color = 0xFF0000FF;    break; //red
            case 2:  color = 0xFF00FF00;  break; //green
            case 3:  color = 0xFFFF0000;   break; //blue
            case 4:  color = 0xFFFFFFFF;  break; //white
            default: color = 0xFF00FFFF; break; //yellow
        }

        //give x and y sides different brightness
        if(side == 1) {color = color / 2;}

        for(int y = drawStart; y < drawEnd; y++) {
            display_draw_pixel(pixelmap, x, y, color);
        }

    }
}

/**
* Cohen-Sutherland Algorithm for clipping 2D lines.
* 
* Screen is divided into sectors depending on x and y values one can determine which sector the point is in.
* The CLIP_CODEs serve a purpose of being addable on top of each other so that N & E = NE.
* Once the sector has been determined for each point the new intersection points are calculated given.
*/
int clip_line(Vector* v1, Vector* v2) {
    
    #define CLIP_CODE_C 0x0000
    #define CLIP_CODE_N 0x0008
    #define CLIP_CODE_S 0x0004
    #define CLIP_CODE_E 0x0002
    #define CLIP_CODE_W 0x0001
    #define CLIP_CODE_NE 0x000a
    #define CLIP_CODE_SE 0x0006
    #define CLIP_CODE_NW 0x0009
    #define CLIP_CODE_SW 0x0005

    int min_clip_y=0, max_clip_y=216-1;
    int min_clip_x=0, max_clip_x=384-1;   

    int x1 = v1->x;
    int y1 = v1->y;
    int x2 = v2->x;
    int y2 = v2->y;

    //CLIP_CODE_C == 0
    int p1_clip_code=0, p2_clip_code=0;

    //check y1 and x1 positions
    if(y1 < min_clip_y) { p1_clip_code |= CLIP_CODE_S; } 
    if(y1 > max_clip_y) { p1_clip_code |= CLIP_CODE_N; }
    if(x1 < min_clip_x) { p1_clip_code |= CLIP_CODE_W; }
    if(x1 > max_clip_x) { p1_clip_code |= CLIP_CODE_E; }

    //check y2 and x2 positions
    if(y2 < min_clip_y) { p2_clip_code |= CLIP_CODE_S; }
    if(y2 > max_clip_y) { p2_clip_code |= CLIP_CODE_N; }
    if(x2 < min_clip_x) { p2_clip_code |= CLIP_CODE_W; }
    if(x2 > max_clip_x) { p2_clip_code |= CLIP_CODE_E; }

    //totally outside frame.
    if((p1_clip_code & p2_clip_code)) {
        return 0;
    }

    if(p1_clip_code == 0 && p2_clip_code == 0) {
        //both vectors lie within frame.
        return 1;
    }


    switch (p1_clip_code) 
    {
        case CLIP_CODE_C: break;

        case CLIP_CODE_N: //north and south inverted from textbook.
            {
                y1 = max_clip_y;
                x1 = x1 + 0.5 + (max_clip_y - y1) * ((x2 - x1) / (y2 - y1));
            } break;
        case CLIP_CODE_S:
            {
                y1 = min_clip_y;
                x1 = x1 + 0.5 + (min_clip_y - y1) * ((x2 - x1) / (y2 - y1));
            } break;
        case CLIP_CODE_W:
            {
                x1 = min_clip_x;
                y1 = y1 + 0.5 + (min_clip_x - x1) * ((y2 - y1) / (x2 - x1));
            } break;
        case CLIP_CODE_E:
            {
                x2 = min_clip_x;
                y2 = y2 + (min_clip_x - x2) * ((y1 -y2) / (x1 - x2));
            } break;
        case CLIP_CODE_NE:
            {
                // north hline intersection
                y1 = max_clip_y;
                x1 = x1 + 0.5 + (max_clip_y - y1) * ((x2 - x1) / (y2 - y1));

                if(x1 < min_clip_x || x1 > max_clip_x)
                {
                    // east vline intersection
                    x1 = max_clip_x;
                    y1 = y1 + 0.5 + (max_clip_x - x1) * ((y2 - y1) / (x2 - x1));
                }
            } break;
        case CLIP_CODE_SE:
            {
                // south hline intersection
                y1 = min_clip_y;
                x1 = x1 + 0.5 + (min_clip_y - y1) * ((x2 - x1) / (y2 - y1));
                if(x1 < min_clip_x || x1 > max_clip_x)
                {
                    // east vline intersection
                    x1 = max_clip_x;
                    y1 = y1 + 0.5 + (max_clip_x - x1) * ((y2 - y1) / (x2 - x1));
                }
            }
        case CLIP_CODE_NW:
            {
                y1 = max_clip_y;
                x1 = x1 + 0.5 + (max_clip_y - y1) * ((x2 - x1) / (y2 - y1));

                if(x1 < min_clip_x || x1 > max_clip_x)
                {
                    x1 = min_clip_x;
                    y1 = y1 + 0.5 + (min_clip_x - x1) * ((y2 - y1) / (x2 - x1));
                }
            } break;
        case CLIP_CODE_SW:
            {
                y1 = min_clip_y;
                x1 = x1 + 0.5 + (min_clip_y - y1) * ((x2 - x1) / (y2 - y1));
                
                if(x1 < min_clip_x || x1 > max_clip_x)
                {
                    x1 = min_clip_x;
                    y1 = y1 + 0.5 + (min_clip_x - x1) * ((y2 - y1) / (x2 - x1));
                }
            } break;
        
        default: break;
    }

    switch (p2_clip_code) {
        case CLIP_CODE_C: break;

        case CLIP_CODE_N:
            {
                //north hline intersection
                y2 = max_clip_y;
                x2 = x2 + (max_clip_y - y2) * ((x1 - x2) / (y1 - y2));
            } break;
        case CLIP_CODE_S:
            {
                // south line intersection
                y2 = min_clip_y;
                x2 = x2 + (min_clip_y - y2) * ((x1 - x2) / (y1 - y2));

            } break;
        case CLIP_CODE_W:
            {
                x2 = min_clip_x;
                y2 = y2 + (min_clip_x - x2) * ((y1 - y2) / (x1 - x2));
            } break;
        case CLIP_CODE_E:
            {
                x2 = max_clip_x;
                y2 = y2 + (max_clip_x - x2) * ((y1 - y2) / (x1 - x2));
            } break;
        case CLIP_CODE_NE:
            {
                // north hline intersection
                y2 = max_clip_y;
                x2 = x2 + (max_clip_y - y2) * ((x1 - x2) / (y1 - y2));

                if(x2 < min_clip_x || x2 > max_clip_x)
                {
                    x2 = max_clip_x;
                    y2 = y2 + 0.5 + (max_clip_x - x2) * ((y1 - y2) / (x1 - x2));
                }
            } break;
        case CLIP_CODE_SE:
            {
                //south hline intersection
                y2 = min_clip_y;
                x2 = x2 + (min_clip_y - y2) * ((x1 - x2) / (y1 - y2));

                if(x2 < min_clip_x || x2 > max_clip_x)
                {
                    //east vline intersection
                    x2 = max_clip_x;
                    y2 = y2 + 0.5 + (max_clip_x - x2) * ((y1 - y2) / (x1 - x2));
                }
            } break;
        case CLIP_CODE_NW:
            {
                // north hline intersection
                y2 = max_clip_y;
                x2 = x2 + (max_clip_y - y2) * ((x1 - x2) / (y1 - y2));

                if(x2 < min_clip_x || x2 > max_clip_x)
                {
                    // west vline intersection
                    x2 = min_clip_x;
                    y2 = y2 + 0.5 + (min_clip_x - x2) * ((y1 - y2) / (x1 - x2));
                }
            } break;
        case CLIP_CODE_SW:
            {
                // south hline intersection
                y2 = min_clip_y;
                x2 = x2 + (min_clip_y - y2) * ((x1 - x2) / (y1 - y2));

                if(x2 < min_clip_x || x2 > max_clip_x)
                {
                    // west vline intersection
                    x2 = min_clip_x;
                    y2 = y2 + 0.5 + (min_clip_x - x2) * ((y1 - y2) / (x1 - x2));
                }
            } break;
    }

    //bounds check;
    if((x1 < min_clip_x) || (x1 > max_clip_x) ||
       (y1 < min_clip_y) || (y1 > max_clip_y) ||
       (x2 < min_clip_x) || (x2 > max_clip_x) ||
       (y2 < min_clip_y) || (y2 > max_clip_y) )
    {
        return 0;
    }

    v1->x = x1;
    v1->y = y1;
    v2->x = x2;
    v2->y = y2;

    return 1;
}

