#include "polygon.h"

// this function draws a triangle that has a flat top
void draw_tb_triangle_3d_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color, uint32_t* pixelmap, int *z_buffer) 
{
    float dx_right,     // the dx/dy ratio of the right edge of line
          dx_left,      // the dx/dy ratio of the left edge of line
          xs,xe,        // the starting and ending points of the edges
          height,       // the height of the triangle

          dx,           // general delta's
          dy,
          z_left,       // the z value of the left edge of current line
          z_right,      // the z value of the right edge of current line
          ay,           // interpolator constant
          b1y,          // the change of z with respect to y on the left edge
          b2y;          // the change of z with respect to y on the right edge

    int temp_x,         // used during sorting as temps
        //temp_y,
        temp_z,
        xs_clip,        // used by clipping
        xe_clip,
        x_index,        // used as looping vars
        y_index;

    // change these 2 back to float and remove all *32 and >>5
    // if you dont want to use fixed point during horizontal interpolation
    float z_middle,       // the z value of the middle between the left and right
        bx;             // the change of z with respect to x


    // test order of x1 and x2, note y1 == y2
    // test if top or bottom is flat and set constant appropriately
    if(y1 == y2)    {
        //perform computations for a triangle with a flat top
        if(x2 < x1) {
            temp_x = x2;
            temp_z = z2;
            x2 = x1;
            z2 = z1;
            x1 = temp_x;
            z1 = temp_z;
        } // end if swap
        // compute deltas for scan conversion
        height = y3 - y1;
        dx_left = (x3 - x1) / height;
        dx_right = (x3 - x2) / height;

        // compute deltas for z interpolation
        z_left  = z1;
        z_right = z2;

        // vertical interpolants
        ay = 1 / height;
        b1y = ay * (z3 - z1);
        b2y = ay * (z3 - z2);

        // set starting points
        xs = (float) x1;
        xe = (float) x2;

    } // end top is flat
    else {
        // bottom must be flat
        // test order of x3 and x2, note y2 == y3
        if(x3 < x2) {
            temp_x = x2;
            temp_z = z2;
            x2 = x3;
            z2 = z3;
            x3 = temp_x;
            z3 = temp_z;
        } // end if swap
        // compute deltas for scan conversion
        height = y3 - y1;
        dx_left = (x2 - x1) / height;
        dx_right = (x3 - x1) / height;

        // compute deltas for z interpolation
        z_left  = z1;
        z_right = z1;

        // vertical interpolants
        ay = 1 / height;
        b1y = ay * (z2 - z1);
        b2y = ay * (z3 - z1);

        // set starting points
        xs = (float) x1;
        xe = (float) x1;
    } // end else bottom is flat

    // perform y clipping
    // clip top
    if(y1 < poly_clip_min_y) {
        // compute new xs and ys
        dy = (float)(-y1 + poly_clip_min_y);
        xs = xs + dx_left * dy;
        xe = xe + dx_right * dy;

        // re-compute z_left and z_right to take into consideration
        // vertical shift down
        z_left  += b1y * dy;
        z_right += b2y * dy;

        // reset y1
        y1 = poly_clip_min_y;
    } // end if top is off screen

    // clip bottom
    if(y3 > poly_clip_max_y) {
        y3 = poly_clip_max_y;
    }

    // test if x clipping is needed
    if(x1 >= poly_clip_min_x && x1 <= poly_clip_max_x &&
       x2 >= poly_clip_min_x && x2 <= poly_clip_max_x &&
       x3 >= poly_clip_min_x && x3 <= poly_clip_max_x)
    {
        // draw the triangle
        for(y_index = y1; y_index <= y3; y_index++) {
            z_middle = z_left;
            bx = (z_right - z_left) / (1 + xe - xs);
            for(x_index = (int) xs; x_index <= (int) xe; x_index++)
            {
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index]) {
                    // update z buffer and write to image buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int) z_middle;
                    display_draw_pixel(pixelmap, x_index, y_index, color);
                } // end if update buffer
                // update current z value
                z_middle += bx;
            } // end draw z buffered line

            // adjust starting point and edning point for scan conversion
            xs += dx_left;
            xe += dx_right;

            // adjust vertical z interpolants
            z_left += b1y;
            z_right += b2y;
        } // end for 
    } // end if no x clipping needed
    else {
        // clip x axis with slower version
        // draw the triangle
        for(y_index = y1; y_index <= y3; y_index++) 
        {
            // do x clip
            xs_clip = (int) xs;
            xe_clip = (int) xe;

            // compute horizontal z interpolant
            z_middle = z_left;
            bx = (z_right - z_left) / (1 + xe - xs);

            // adjust starting point and ending point
            xs += dx_left;
            xe += dx_right;

            // adjust vertical z interpolants
            z_left += b1y;
            z_right += b2y;

            // clip line
            if(xs_clip < poly_clip_min_x)  {
                dx = (-xs_clip + poly_clip_min_x);
                xs_clip = poly_clip_min_x;

                // re-compute z_middle to take into consideration horizontal shift
                z_middle += (bx * dx);
            } // end if line is clipp on left

            if(xe_clip > poly_clip_max_x) {
                xe_clip = poly_clip_max_x;
            } // ned if line is clipped on right

            // draw the line
            for(x_index = xs_clip; x_index <= xe_clip; x_index++)
            {
                // if current z_middle is less than z-buffer then replace
                // and update image buffer
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index]) {
                    // update z buffer and write to image buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int) z_middle;
                    display_draw_pixel(pixelmap, x_index, y_index, color);
                } // end if update z buffer
                // update current z value
                z_middle += bx;
            } // end draw z buffered line
        } // end for y_index

    } // ned else x clipping needed
}


// Draws Triangles by determining float top or bottom triangle. 
// Same as draw_triangle_2D() except that this function incorporates a Z-buffer.
void draw_triangle_3D_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color[4], uint32_t* pixelmap, int *z_buffer, int mode) 
{
    int temp_x,     // used for sorting
        temp_y,
        temp_z,
        temp_i,
        new_x,      // used to compute new x and z at triangle splitting point
        new_z,
        i1,
        i2,
        i3;

    i1 = color[0];
    i2 = color[1];
    i3 = color[2];

    // test for h lines and v lines
    if((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3)) {
        return;
    }

    // sort p1, p2, p3 in ascending y order
    if(y2 < y1) {
        temp_x = x2;
        temp_y = y2;
        temp_z = z2;
        temp_i = i2;
        x2 = x1;
        y2 = y1;
        z2 = z1;
        i2 = i1;
        x1 = temp_x;
        y1 = temp_y;
        z1 = temp_z;
        i1 = temp_i;
    }

    // now we know that p1 and p2 are in order
    if(y3 < y1) {
        temp_x = x3;
        temp_y = y3;
        temp_z = z3;
        temp_i = i3;
        x3 = x1;
        y3 = y1;
        z3 = z1;
        i3 = i1;
        x1 = temp_x;
        y1 = temp_y;
        z1 = temp_z;
        i1 = temp_i;
    }

    // finally test y3 against y2
    if(y3 < y2) {
        temp_x = x3;
        temp_y = y3;
        temp_z = z3;
        temp_i = i3;
        x3 = x2;
        y3 = y2;
        z3 = z2;
        i3 = i2;
        x2 = temp_x;
        y2 = temp_y;
        z2 = temp_z;
        i2 = temp_i;
    }

    // do trivial rejection tests
    if(y3 < poly_clip_min_y || y1 > poly_clip_max_y ||
       (x1 < poly_clip_min_x && x2 < poly_clip_min_x && x3 < poly_clip_min_x) ||
       (x1 > poly_clip_max_x && x2 > poly_clip_max_x && x3 > poly_clip_max_x)) {
        return;
    }

    // test if top of triangle is flat
    if(y1 == y2 || y2 == y3) {
        if(mode == FLAT_SHADING) {
            draw_tb_triangle_3d_z(x1,y1,z1,x2,y2,z2,x3,y3,z3,color[0],pixelmap, z_buffer);
        }
        if(mode == GOURAUD_SHADING) {
            draw_tb_triangle_3d_gouraud(x1,y1,z1, i1 ,x2,y2,z2, i2, x3,y3,z3, i3, pixelmap, z_buffer);
        }
    }
    else  {
        // general tirangle that needs to be borken up along long edge
        // compute new x,z at split point
        new_x = x1 + (int)((float)(y2 - y1) * (float)(x3 - x1) / (float)(y3 - y1));
        new_z = z1 + (int)((float)(y2 - y1) * (float)(z3 - z1) / (float)(y3 - y1));

        // determine intensity light of new position
        int new_i, new_i_b,
            new_i_g, new_i_r;

        int i1_b, i1_g, i1_r,
            i3_b, i3_g, i3_r;

        i1_b = (i1 >> 16) & 0xFF;
        i1_g = (i1 >> 8) & 0xFF;
        i1_r = (i1 & 0xFF);

        i3_b = (i3 >> 16) & 0xFF;
        i3_g = (i3 >> 8) & 0xFF;
        i3_r = (i3 & 0xFF);
        
        new_i_b = i1_b + (int)((float)(y2 - y1) * (float)(i3_b - i1_b) / (float)(y3 - y1));
        new_i_g = i1_g + (int)((float)(y2 - y1) * (float)(i3_g - i1_g) / (float)(y3 - y1));
        new_i_r = i1_r + (int)((float)(y2 - y1) * (float)(i3_r - i1_r) / (float)(y3 - y1));
        new_i = _RGB32BIT(0, new_i_r, new_i_g, new_i_b);

        // draw each sub-triangle
        if(y3 >= poly_clip_min_y && y1 < poly_clip_max_y) {
            if(mode == FLAT_SHADING) {
                draw_tb_triangle_3d_z(x2,y2,z2,new_x,y2,new_z,x3,y3,z3,color[0],pixelmap, z_buffer);
            }
            if(mode == GOURAUD_SHADING) {
                draw_tb_triangle_3d_gouraud(x2,y2,z2,i2, new_x,y2,new_z, new_i, x3,y3,z3, i3 ,pixelmap, z_buffer); // upper triangle 
            }
        }
        if(y2 >= poly_clip_min_y && y1 < poly_clip_max_y) {
            if(mode == FLAT_SHADING) {
                draw_tb_triangle_3d_z(x1,y1,z1,new_x,y2,new_z,x2,y2,z2,color[0],pixelmap, z_buffer);
            }
            if(mode == GOURAUD_SHADING) {
                draw_tb_triangle_3d_gouraud(x1,y1,z1, i1, new_x,y2,new_z,new_i ,x2,y2,z2, i2, pixelmap, z_buffer); // lower triangle
            }
        }
    }
}


// Extra shading function that breaks the triangle down using interpolation
// into even smaller areas. These areas then use a shading from 0-63 steps to 
// Achieve a finer look. Requires specific intensity.
void draw_tb_triangle_3d_gouraud(int x1, int y1, int z1, int i1,
                        int x2, int y2, int z2, int i2,
                        int x3, int y3, int z3, int i3, 
                        uint32_t* pixelmap, int *z_buffer) 
{
    // this function draws a triangle that has a flat top
    float dx_right,     // the dx/dy ratio of the right edge of line
          dx_left,      // the dx/dy ratio of the left edge of line
          xs,xe,        // the starting and ending points of the edges
          height,       // the height of the triangle

          i_b_left,
          i_b_right,
          i_g_left,
          i_g_right,
          i_r_left,
          i_r_right,

          dx,           // general delta's
          dy,
          z_left,       // the z value of the left edge of current line
          z_right,      // the z value of the right edge of current line
          ay,           // interpolator constant
          b1y,          // the change of z with respect to y on the left edge
          b2y,          // the change of z with respect to y on the right edge
          b1y_i_b,
          b2y_i_b,
          b1y_i_g,
          b2y_i_g,
          b1y_i_r,
          b2y_i_r;


    int temp_x,         // used during sorting as temps
        temp_z,
        temp_i,
        xs_clip,        // used by clipping
        xe_clip,
        x_index,        // used as looping vars
        y_index;

    // change these 2 back to float and remove all *32 and >>5
    // if you dont want to use fixed point during horizontal interpolation
    float z_middle,       // the z value of the middle between the left and right
        i_b_middle,
        i_g_middle,
        i_r_middle,
        bx,            // the change of z with respect to x
        i_b_x,
        i_g_x,
        i_r_x;             

    // test order of x1 and x2, note y1 == y2
    int i1_b, i1_g, i1_r,
        i2_b, i2_g, i2_r,
        i3_b, i3_g, i3_r;

    // test if top or bottom is flat and set constant appropriately
    if(y1 == y2) {
        //perform computations for a triangle with a flat top
        if(x2 < x1) {
            temp_x = x2;
            temp_z = z2;
            temp_i = i2;
            x2 = x1;
            z2 = z1;
            i2 = i1;
            x1 = temp_x;
            z1 = temp_z;
            i1 = temp_i;
        } // end if swap

        i1_b = (i1 >> 16) & 0xFF;
        i1_g = (i1 >> 8) & 0xFF;
        i1_r = (i1) & 0xFF;

        i2_b = (i2 >> 16) & 0xFF;
        i2_g = (i2 >> 8) & 0xFF;
        i2_r = (i2) & 0xFF;

        i3_b = (i3 >> 16) & 0xFF;
        i3_g = (i3 >> 8) & 0xFF;
        i3_r = (i3) & 0xFF;

        // compute deltas for scan conversion
        height = y3 - y1;
        dx_left = (x3 - x1) / height;
        dx_right = (x3 - x2) / height;

        // compute deltas for z interpolation
        z_left  = z1;
        z_right = z2;
        i_b_left  = i1_b; // blue
        i_b_right = i2_b;
        i_g_left  = i1_g; // green
        i_g_right = i2_g;
        i_r_left  = i1_r; // red
        i_r_right = i2_r; 

        // vertical interpolants
        ay = 1 / height;
        b1y = ay * (z3 - z1);
        b2y = ay * (z3 - z2);
        b1y_i_b = ay * (i3_b - i1_b); // blue
        b2y_i_b = ay * (i3_b - i2_b);
        b1y_i_g = ay * (i3_g - i1_g); // green
        b2y_i_g = ay * (i3_g - i2_g);
        b1y_i_r = ay * (i3_r - i1_r); // red
        b2y_i_r = ay * (i3_r - i2_r);

        // set starting points
        xs = (float) x1;
        xe = (float) x2;

    } // end top is flat
    else  {
        // bottom must be flat
        // test order of x3 and x2, note y2 == y3
        if(x3 < x2) {
            temp_x = x2;
            temp_z = z2;
            temp_i = i2;
            x2 = x3;
            z2 = z3;
            i2 = i3;
            x3 = temp_x;
            z3 = temp_z;
            i3 = temp_i;
        } // end if swap

        i1_b = (i1 >> 16) & 0xFF;
        i1_g = (i1 >> 8) & 0xFF;
        i1_r = (i1) & 0xFF;

        i2_b = (i2 >> 16) & 0xFF;
        i2_g = (i2 >> 8) & 0xFF;
        i2_r = (i2) & 0xFF;

        i3_b = (i3 >> 16) & 0xFF;
        i3_g = (i3 >> 8) & 0xFF;
        i3_r = (i3) & 0xFF;

        // compute deltas for scan conversion
        height = y3 - y1;

        dx_left = (x2 - x1) / height;
        dx_right = (x3 - x1) / height;

        // compute deltas for z interpolation
        z_left  = z1;
        z_right = z1;
        i_b_left  = i1_b; // blue
        i_b_right = i1_b;
        i_g_left  = i1_g; // green
        i_g_right = i1_g;
        i_r_left  = i1_r; // red
        i_r_right = i1_r;

        // vertical interpolants
        ay = 1 / height;
        b1y = ay * (z2 - z1);
        b2y = ay * (z3 - z1);
        b1y_i_b = ay * (i2_b - i1_b); // blue
        b2y_i_b = ay * (i3_b - i1_b);
        b1y_i_g = ay * (i2_g - i1_g); // green
        b2y_i_g = ay * (i3_g - i1_g);
        b1y_i_r = ay * (i2_r - i1_r); // red
        b2y_i_r = ay * (i3_r - i1_r);

        // set starting points
        xs = (float) x1;
        xe = (float) x1;

    } // end else bottom is flat
    
    // perform y clipping
    // clip top
    if(y1 < poly_clip_min_y) {
        // compute new xs and ys
        dy = (float)(-y1 + poly_clip_min_y);
        xs = xs + dx_left * dy;
        xe = xe + dx_right * dy;

        // re-compute z_left and z_right to take into consideration
        // vertical shift down
        z_left  += b1y * dy;
        z_right += b2y * dy;
        i_b_left  += b1y_i_b * dy; // blue
        i_b_right += b2y_i_b * dy;
        i_g_left  += b1y_i_g * dy; // green
        i_g_right += b2y_i_g * dy;
        i_r_left  += b1y_i_r * dy; // red
        i_r_right += b2y_i_r * dy;

        // reset y1
        y1 = poly_clip_min_y;

    } // end if top is off screen
    // clip bottom
    if(y3 > poly_clip_max_y) {
        y3 = poly_clip_max_y;
    }

    // test if x clipping is needed
    if(x1 >= poly_clip_min_x && x1 <= poly_clip_max_x &&
       x2 >= poly_clip_min_x && x2 <= poly_clip_max_x &&
       x3 >= poly_clip_min_x && x3 <= poly_clip_max_x)
    {
        // draw the triangle
        for(y_index = y1; y_index <= y3; y_index++)
        {
            // z_middle set to float
            z_middle = z_left;
            bx = (z_right - z_left) / (1 + xe - xs);

            i_b_middle = i_b_left;
            i_b_x = (i_b_right - i_b_left) / (1 + xe - xs);
            i_g_middle = i_g_left;
            i_g_x = (i_g_right - i_g_left) / (1 + xe - xs);
            i_r_middle = i_r_left;
            i_r_x = (i_r_right - i_r_left) / (1 + xe - xs);

            for(x_index = (int) xs; x_index <= (int) xe; x_index++)
            {
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index]) {
                    // update z buffer and write to image buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int)(z_middle);
                    int rgb = _RGB32BIT(0, (int)(i_r_middle),(int)(i_g_middle), (int)(i_b_middle));
                    display_draw_pixel(pixelmap, x_index, y_index, rgb);
                } // end if update buffer
                // update current z value
                z_middle += bx;
                i_b_middle += i_b_x;
                i_g_middle += i_g_x;
                i_r_middle += i_r_x;
            } // end draw z buffered line

            // adjust starting point and edning point for scan conversion
            xs += dx_left;
            xe += dx_right;

            // adjust vertical z interpolants
            z_left += b1y;
            z_right += b2y;
            i_b_left  += b1y_i_b; // blue
            i_b_right += b2y_i_b;
            i_g_left  += b1y_i_g; // green
            i_g_right += b2y_i_g;
            i_r_left  += b1y_i_r; // red
            i_r_right += b2y_i_r;
        } // end for 
    } // end if no x clipping needed
    else {
        // clip x axis with slower version
        // draw the triangle
        for(y_index = y1; y_index <= y3; y_index++)
        {
            // do x clip
            xs_clip = (int)(xs + 0.5);
            xe_clip = (int)(xe + 0.5);

            // compute horizontal z interpolant
            z_middle = z_left;
            bx = (z_right - z_left) / (1 + xe - xs);
            i_b_middle = i_b_left;
            i_b_x = (i_b_right - i_b_left) / (1 + xe - xs);
            i_g_middle = i_g_left;
            i_g_x = (i_g_right - i_g_left) / (1 + xe - xs);
            i_r_middle = i_r_left;
            i_r_x = (i_r_right - i_r_left) / (1 + xe - xs);

            // adjust starting point and ending point
            xs += dx_left;
            xe += dx_right;

            // adjust vertical z interpolants
            z_left += b1y;
            z_right += b2y;
            i_b_left  += b1y_i_b; // blue
            i_b_right += b2y_i_b;
            i_g_left  += b1y_i_g; // green
            i_g_right += b2y_i_g;
            i_r_left  += b1y_i_r; // red
            i_r_right += b2y_i_r;

            // clip line
            if(xs_clip < poly_clip_min_x) {
                dx = (-xs_clip + poly_clip_min_x);
                xs_clip = poly_clip_min_x;

                // re-compute z_middle to take into consideration horizontal shift
                z_middle += (bx * dx);
                i_b_middle += (i_b_x * dx);
                i_g_middle += (i_g_x * dx);
                i_r_middle += (i_r_x * dx);
            } // end if line is clipp on left

            if(xe_clip > poly_clip_max_x) {
                xe_clip = poly_clip_max_x;
            } // ned if line is clipped on right

            // draw the line
            for(x_index = xs_clip; x_index <= xe_clip; x_index++)
            {
                // if current z_middle is less than z-buffer then replace
                // and update image buffer
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index]) {
                    // update z buffer and write to image buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int)(z_middle);
                    int rgb = _RGB32BIT(0, (int)(i_r_middle), (int)(i_g_middle), (int)(i_b_middle));
                    display_draw_pixel(pixelmap, x_index, y_index, rgb);
                } // end if update z buffer

                // update current z value
                z_middle += bx;
                i_b_middle += i_b_x;
                i_g_middle += i_g_x;
                i_r_middle += i_r_x;
            } // end draw z buffered line
        } // end for y_index
    } // ned else x clipping needed
}