#include "polygon.h"
#include "vector.h"
#include "../integration/display.h"
#include <stdio.h>



#define poly_clip_min_y 0
#define poly_clip_max_y (216 - 1)
#define poly_clip_min_x 0
#define poly_clip_max_x (384 - 1)


Triangle triangle_create(const Vector* v1, const Vector* v2, const Vector* v3, int color) {
    Triangle triangle;
    triangle.v[0] = *v1;
    triangle.v[1] = *v2;
    triangle.v[2] = *v3;
    triangle.color = color;
    return triangle;
}

void triangle_draw_2D(Triangle* triangle, uint32_t* pixelmap) {

    int x1 = triangle->v[0].x, y1 = triangle->v[0].y;
    int x2 = triangle->v[1].x, y2 = triangle->v[1].y;
    int x3 = triangle->v[2].x, y3 = triangle->v[2].y;
    int color = triangle->color;


    int temp_x,
        temp_y,
        new_x;

    // test for h lines and v lines
    if((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3)) {
        return;
    }

    //sort p1, p2, p3 in ascending y order
    if(y2 < y1)
    {
        temp_x = x2;
        temp_y = y2;
        x2 = x1;
        y2 = y1;
        x1 = temp_x;
        y1 = temp_y;
    }

    // now we know that p1 and p2 are in order
    if(y3 < y1)
    {
        temp_x = x3;
        temp_y = y3;
        x3 = x1;
        y3 = y1;
        x1 = temp_x;
        y1 = temp_y;
    }

    // finally test y3 against y2
    if(y3 < y2)
    {
        temp_x = x3;
        temp_y = y3;
        x3 = x2;
        y3 = y2;
        x2 = temp_x;
        y2 = temp_y;
    }

    // do trivial rejection tests (entire triangle outside frame)
    if( y3 < poly_clip_min_y || y1 > poly_clip_max_y ||
        (x1 < poly_clip_min_x && x2 < poly_clip_min_x && x3 < poly_clip_min_x) ||
        (x1 > poly_clip_max_x && x2 > poly_clip_max_x && x3 > poly_clip_max_x) )
    {
        return;
    }

    // test if top of triangle is flat
    if(y1 == y2)
    {
        draw_top_triangle(x1,y1,x2,y2,x3,y3,color,pixelmap);
    }
    else
    if(y2 == y3)
    {  
        draw_bottom_triangle(x1,y1,x2,y2,x3,y3,color,pixelmap);
    }
    else 
    {
        //general triangle that needs to be broken up along long edge.
        new_x = x1 + (int) ((float)(y2 - y1) * ((float)(x3-x1) / (float)(y3 - y1)));

        draw_bottom_triangle(x1, y1, new_x, y2, x2, y2, color, pixelmap);
        draw_top_triangle(x2, y2, new_x, y2, x3, y3, color, pixelmap);
    
    }

}

void draw_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap)
{

    float dx_right, // the dx/dy ratio of the right edge of line
          dx_left,  // the dx/dy ratio of the left edge of line
          xs,xe,    // the starting and ending points of the edges
          height;   // the height of the triangle

    int temp_x,
        temp_y,
        right,
        left;

    // test order 
    if(x2 < x1)
    {
        temp_x = x2;
        x2 = x1;
        x1 = temp_x;
    }

    // copte deltas
    height = y3 - y1;

    dx_left = (x3 - x1) / height;
    dx_right =(x3 - x2) / height;

    // set starting points
    xs = (float) x1;
    xe = (float) x2 + (float) 0.5;

    // perform y clipping
    // top is of screen.
    if(y1 < poly_clip_min_y)
    {
        // compute new xs and ys
        xs = xs + dx_left * (float) (-y1 + poly_clip_min_y);
        xe = xe + dx_right * (float) (-y1 + poly_clip_min_y);

        // reset y1
        y1 = poly_clip_min_y;
    }

    if(y3 > poly_clip_max_y) {
        y3 = poly_clip_max_y;
    }

    // test if x clipping is needed
    if(x1 >= poly_clip_min_x && x1 <= poly_clip_max_x &&
       x2 >= poly_clip_min_x && x2 <= poly_clip_max_x &&
       x3 >= poly_clip_min_x && x3 <= poly_clip_max_x )
    {
        for(temp_y = y1; temp_y <= y3; temp_y++)
        {
            for(int x = (int) xs; x < (int) xe; x++)
            {
                display_draw_pixel(pixelmap, x, temp_y, color);
            }

            xs+=dx_left;
            xe+=dx_right;
        }
    }
    else
    {
        //clip x axis with slower version
        for(temp_y = y1; temp_y <= y3; temp_y++)
        {
            // do x clip
            left = (int) xs;
            right = (int) xe;

            // adjust starting point and ending point
            xs += dx_left;
            xe += dx_right;

            // clip line
            if(left < poly_clip_min_x)
            {
                left = poly_clip_min_x;
                if(right < poly_clip_min_x) {
                    continue;
                }
            }

            if(right > poly_clip_min_x)
            {
                right = poly_clip_max_x;
                if(left > poly_clip_max_x) {
                    continue;
                }
            }

            for(int x = (int) xs; x < (int) xe; x++)
            {
                display_draw_pixel(pixelmap, x, temp_y, color);
            }

        }
    }
}   


void draw_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap) 
{

    float dx_right, // the dx/dy ratio of the right edge of line
          dx_left,  // the dx/dy ratio of the left edge of line
          xs,xe,    // the starting and ending points of the edges
          height;   // the height of the triangle

    int temp_x,
        temp_y,
        right,
        left;

    // test order of x1 and x2
    if(x3 < x2)
    {
        temp_x = x2;
        x2 = x3;
        x3 = temp_x;
    }

    //compute deltas
    height = y3 - y1;

    dx_left = (x2 - x1) / height;
    dx_right = (x3 - x1) / height;

    // set starting points
    xs = (float) x1;
    xe = (float) x1 + (float) 0.5;

    // perform y clipping
    if(y1 < poly_clip_min_y)
    {
        // compute new xs and ys
        xs = xs + dx_left * (float)(-y1 + poly_clip_min_y);
        xe = xe + dx_right * (float)(-y1 + poly_clip_min_y);

        y1 = poly_clip_min_y;
    }

    if(y3>poly_clip_max_y) {
        y3=poly_clip_max_y;
    }

    // test if x clipping is needed
    if(x1 >= poly_clip_min_x && x1 <= poly_clip_max_x &&
       x2 >= poly_clip_min_x && x2 <= poly_clip_max_x &&
       x3 >= poly_clip_min_x && x3 <= poly_clip_max_x )
    {
        for(temp_y = y1; temp_y <= y3; temp_y++)
        {
            for(int x = (int) xs; x < (int) xe; x++)
            {
                display_draw_pixel(pixelmap, x, temp_y, color);
            }

            xs += dx_left;
            xe += dx_right;
        }
    }
    else
    {
        //clip x axis with slower version
        for(temp_y = y1; temp_y <= y3; temp_y++)
        {
            // do x clip

            left = (int) xs;
            right = (int) xe;

            // adjust starting point and ending point
            xs += dx_left;
            xe += dx_right;

            // clip line
            if(left < poly_clip_min_x)
            {
                left = poly_clip_min_x;
                if(right < poly_clip_min_x) {
                    continue;
                }
            }

            if(right > poly_clip_max_x)
            {
                right = poly_clip_max_x;
                if(left > poly_clip_max_x) {
                    continue;
                }
            }

            for(int x = (int) xs; x < (int) xe; x++)
            {
                display_draw_pixel(pixelmap, x, temp_y, color);
            }
        }
    }
}
