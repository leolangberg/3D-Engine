#include "polygon.h"
#include "global.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "../integration/display.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/**
* Draws Flat-top case triangles onto pixel screen.
*/
void draw_top_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap) {

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

/**
* Draws Flat-bottom case triangles onto pixel screen.
*/
void draw_bottom_triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap) {

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

/**
* Draws Triangle on pixel screen.
*
* Algorithm determines whether triangle drawn is one of the following cases:
* Flat top, Flat bottom or General. In case of general the triangle is sorted and 
* split dividing the triangle into 2 now distint flat top or bottom triangles.
*/
void triangle_draw_2D(int x1, int y1, int x2, int y2, int x3, int y3, int color, uint32_t* pixelmap) {

    //printf("draw triangle main\n");

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

/**
* Computes the maximum radius or sphere around object.
*/
float compute_object_radius(Object* object) {
    // this function computes maximum radius of object.

    float new_radius,      // used in average radious calculation of object
          x,y,z;            // a single vertex

    int index;              // looping variable

    // reset object radious 

    object->radius = 0;

    for(index = 0; index < object->num_vertices; index++)
    {
        x = object->vertices_local[index].x;
        y = object->vertices_local[index].y;
        z = object->vertices_local[index].z;

        // compute distance to point
        new_radius = (float) d_sqrt((x*x) + (y*y) + (z*z));

        // is this radius bigger that last?
        if(new_radius > object->radius) {
            object->radius = new_radius;
        }
    }

    return object->radius;
}

/**
* Divides each RGB color value into 16 steps between 0 to 255 (16 each).
* Each color is then applied the new intensity value multiplied by the ratio of each color.
*/
int color_intensity_conversion(int color, int intensity) {

    // Mask to extract each color channel
    int alpha =  color & 0xFF000000,
        blue  = (color & 0x00FF0000) >> 16,
        green = (color & 0x0000FF00) >> 8,
        red   =  color & 0x000000FF;

    // Calculate the sum of the RGB values
    int sum = red + green + blue;

    // Avoid division by zero
    if (sum == 0) {
        sum = 1;
    }

    // value of new color based on intensity 0-255.
    int value = 16 * intensity + 15;

    // Compute the new color channels based on the ratio
    float percentage_red   = (float) red / sum,
          percentage_green = (float) green / sum,
          percentage_blue  = (float) blue / sum;

    red = percentage_red * value;
    green = percentage_green * value;
    blue = percentage_blue * value;

    // Combine the channels back into a single integer
    int new_color = alpha | (blue << 16) | (green << 8) | red;

    return new_color;
}

/**
* Removes backfaces meaning that the method determines if polygons are invisible or clipped from
* the current viewpoint, and thus only draws relevant polygons. 
* Relevant polygons of object are also colored and shaded correctly based on direction 
* to light source.
*/
void remove_backfaces_and_shade(Object* object, Vector* view_point, int mode) {
    // this function removes all the backfaces of a n object by setting the removed
    // flag. This function assumes that the object has been transformed into 
    // camera coordinates. Also, the function computes the flat shading of the 
    // object. 

    int vertex_0,       // vertex indices
        vertex_1,
        vertex_2,
        curr_poly;      // current polygon

    float dp,           // the result of the dot product
          intensity;    // the final intesity of the surface

    Vector u,v,         // general working vectors
           normal,      // the normal to the surface being processed
           sight;       // line of sight vector

    // for each polygon in the object determine if it is pointing away from the
    // viewpoint and direction

    for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
    {

        if(object->polys[curr_poly].two_sided == ONE_SIDED)
        {

            // compute the two vectors on polygon that have the same initial points
            vertex_0 = object->polys[curr_poly].vertex_list[0];
            vertex_1 = object->polys[curr_poly].vertex_list[1];
            vertex_2 = object->polys[curr_poly].vertex_list[2];

            // the vector u = v0->v1
            // the vector v = v0->v2
            u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
            v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);

            // compute the normal to polygon v x u
            normal = *vector_cross_product(&v, &u);

            // compute the line of sight vector, since all coordinates are world all
            // object vertices are already relative to (0,0,0) thus

            sight.x = view_point->x - object->vertices_world[vertex_0].x;
            sight.y = view_point->y - object->vertices_world[vertex_0].y;
            sight.z = view_point->z - object->vertices_world[vertex_0].z;

            // compute the dot product between line of sight vector and normal to surface
            dp = vector_dot_product(&normal, &sight);

            // set the clip flagged appropriately
            if(dp>0)
            {
                // set visibility
                object->polys[curr_poly].visible = 1;

                // compute light intensity if needed
                if(mode == FLAT_SHADING)
                {
                    // compute the dot product between light source vector
                    // and normal vector to surface
                    dp  = vector_dot_product(&normal, &light_source);

                    // test if light ray is reflecting off surface
                    if(dp>0)
                    {
                        // now cos 0 = (u*v) / |u||v| 
                        intensity = ambient_light + ((15*dp) / vector_length(&object->polys[curr_poly].normal));

                        // test if intensity has overflowed
                        if(intensity > 15) {
                            intensity = 15;
                        }

                        // intensity now varies from 0-1, 0 being black or grazing and 1 being
                        // totally illuminated. use the value to index into color table

                        object->polys[curr_poly].shade = color_intensity_conversion(object->polys[curr_poly].color, (int) intensity);
                
                    } // end if light is reflecting off surface
                    else
                    {
                        object->polys[curr_poly].shade = color_intensity_conversion(object->polys[curr_poly].color, (int) intensity);
                    } // end if use flat shading
                }
                else
                {   
                    // assueme constant shading and simply assign color to shade
                    object->polys[curr_poly].shade = object->polys[curr_poly].color;
                } // end else constant shading 
            } // end if face is visible
            else
            {
                object->polys[curr_poly].visible = 0; // set invisible flag
            } 
        }
        else
        {
            // else polygon is always visible i.e. two sided, set visibility flag
            // so negine renders it
            object->polys[curr_poly].visible = 1;

            // perform shading calculation
            // compute the two vectors on polygon that have the same initial points
            vertex_0 = object->polys[curr_poly].vertex_list[0];
            vertex_1 = object->polys[curr_poly].vertex_list[1];
            vertex_2 = object->polys[curr_poly].vertex_list[2];

            // the vector u = v0->v1
            // the vector v = v0->v2
            u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
            v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);

            // compute the normal to polygon v x u
            normal = *vector_cross_product(&v, &u);

            // compute the line of sight vector, since all coordinates are world all
            // object vertices are already relative to (0,0,0) thus

            sight.x = view_point->x - object->vertices_world[vertex_0].x;
            sight.y = view_point->y - object->vertices_world[vertex_0].y;
            sight.z = view_point->z - object->vertices_world[vertex_0].z;

            // compute the dot product between line of sight vector and normal to surface
            dp = vector_dot_product(&normal, &sight);

            // set the clip flagged appropriately
            if(dp>0)
            {
                // set visibility
                object->polys[curr_poly].visible = 1;

                // compute light intensity if needed
                if(mode == FLAT_SHADING)
                {
                    // compute the dot product between light source vector
                    // and normal vector to surface
                    dp  = vector_dot_product(&normal, &light_source);

                    // test if light ray is reflecting off surface
                    if(dp>0)
                    {
                        // now cos 0 = (u*v) / |u||v| 
                        intensity = ambient_light + ((15*dp) / vector_length(&object->polys[curr_poly].normal));

                        // test if intensity has overflowed
                        if(intensity > 15) {
                            intensity = 15;
                        }

                        // intensity now varies from 0-1, 0 being black or grazing and 1 being
                        // totally illuminated. use the value to index into color table

                        object->polys[curr_poly].shade = color_intensity_conversion(object->polys[curr_poly].color, (int) intensity);
                
                    } // end if light is reflecting off surface
                    else
                    {
                        object->polys[curr_poly].shade = color_intensity_conversion(object->polys[curr_poly].color, (int) intensity);
                    } // end if use flat shading
                }
                else
                {   
                    // assueme constant shading and simply assign color to shade
                    object->polys[curr_poly].shade = object->polys[curr_poly].color;
                } // end else constant shading 
            } // end if face is visible
        } // end else two sided
    } // end for curr_poly
}

/**
* All relevant and visible sides of the object are converted into screen space
* and then drawn via triangle_draw_2d() method.
*/
void draw_object_solid(Object* object, uint32_t* pixelmap) {
    // this function draws an object shaded solid and can perform
    // simple z extent clipping.
    // Z-clipping refers to clipping of object that are to far away or
    // behind camera.


    int curr_poly,          // the current polygon
        vertex_1,           // vertex index numbers
        vertex_2,
        vertex_3,
        vertex_4;

    float x1,y1,z1,         // working variables
          x2,y2,z2,
          x3,y3,z3,
          x4,y4,z4;

    // compute position of object in world
    for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
    {
        int is_quad = 0;
        // if polygon is not visible or clipped then continue to next loop iteration
        if(object->polys[curr_poly].visible == 0 || object->polys[curr_poly].clipped) {
            continue;
        }

        // extract the vertex numbers
        vertex_1 = object->polys[curr_poly].vertex_list[0];
        vertex_2 = object->polys[curr_poly].vertex_list[1];
        vertex_3 = object->polys[curr_poly].vertex_list[2];

        // do Z clipping first before projection
        z1 = object->vertices_camera[vertex_1].z;
        z2 = object->vertices_camera[vertex_2].z;
        z3 = object->vertices_camera[vertex_3].z;

        // test if this is a quad
        if(object->polys[curr_poly].num_points == 4)
        {
            // extract vertex number and z component for clipping and projection
            vertex_4 = object->polys[curr_poly].vertex_list[3];
            z4 = object->vertices_camera[vertex_4].z;

            // set quad flag
            is_quad = 1;
        }
        else {
            z4 = z3;
        }

            // perform z clipping test

            if((z1<CLIP_NEAR_Z && z2 < CLIP_NEAR_Z && z3 < CLIP_NEAR_Z && z4 < CLIP_NEAR_Z) || 
            (z1 > CLIP_FAR_Z && z2 > CLIP_FAR_Z && z3 > CLIP_FAR_Z && z4 > CLIP_FAR_Z))
            {
                continue;
            }
        
        // extract points of polygon
        x1 = object->vertices_camera[vertex_1].x;
        y1 = object->vertices_camera[vertex_1].y;

        x2 = object->vertices_camera[vertex_2].x;
        y2 = object->vertices_camera[vertex_2].y;

        x3 = object->vertices_camera[vertex_3].x;
        y3 = object->vertices_camera[vertex_3].y;


        x1 = (((float) WINDOW_WIDTH / 2)  + x1 * VIEWING_DISTANCE / z1);
        y1 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y1 * VIEWING_DISTANCE / z1);

        x2 = (((float) WINDOW_WIDTH / 2)  + x2 * VIEWING_DISTANCE / z2);
        y2 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y2 * VIEWING_DISTANCE / z2);

        x3 = (((float) WINDOW_WIDTH / 2)  + x3 * VIEWING_DISTANCE / z3);
        y3 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y3 * VIEWING_DISTANCE / z3);

        //shade instead of color according to Lamotte.
        triangle_draw_2D((int) x1, (int) y1, (int) x2, (int) y2, (int) x3, (int) y3, object->polys[curr_poly].shade, pixelmap);

        // draw second poly if this is a quad
        if(is_quad)
        {
            // extract the point
            x4 = object->vertices_camera[vertex_4].x;
            y4 = object->vertices_camera[vertex_4].y;

            x4 = (((float) WINDOW_WIDTH / 2)  + x4 * VIEWING_DISTANCE / z4);
            y4 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y4 * VIEWING_DISTANCE / z4);

            triangle_draw_2D((int) x1, (int) y1, (int) x3, (int) y3, (int) x4, (int) y4, object->polys[curr_poly].shade, pixelmap);
        } // end if quad
    } // end for curr_poly
}

/**
* Sets the world position of an object.
*/
void object_position(Object* object, int x, int y, int z) {
    object->world_pos.x = x; object->world_pos.y = y; object->world_pos.z = z;
}

/**
* Rotates object along the y-axis.
*/
void object_rotate_y(Object* object, float angle_rad) {

    int index;
    Matrix m;
    Matrix y = *matrix_create_rotation_matrix_y(angle_rad);

    for(index = 0; index < object->num_vertices; index++)
    {
        m = *vector_as_matrix(&object->vertices_local[index]);
        m = *matrix_mul(&m, &y);
        object->vertices_local[index].x = m.matrix[0][0];
        object->vertices_local[index].y = m.matrix[0][1];
        object->vertices_local[index].z = m.matrix[0][2];
    }

}

/**
* Rotates object along the z-axis.
*/
void object_rotate_z(Object* object, float angle_rad) {

    int index;
    Matrix m;
    Matrix z = *matrix_create_rotation_matrix_z(angle_rad);

    for(index = 0; index < object->num_vertices; index++)
    {
        m = *vector_as_matrix(&object->vertices_local[index]);
        m = *matrix_mul(&m, &z);
        object->vertices_local[index].x = m.matrix[0][0];
        object->vertices_local[index].y = m.matrix[0][1];
        object->vertices_local[index].z = m.matrix[0][2];
    }

}

/**
* Transforms local coordinates to world coordinates by simply translating (adding) world pos with
* local coordinates.
*/
void object_local_to_world_transformation(Object* object) {

    for(int index = 0; index < object->num_vertices; index++)
    {
        object->vertices_world[index].x = object->vertices_local[index].x + object->world_pos.x;
        object->vertices_world[index].y = object->vertices_local[index].y + object->world_pos.y;
        object->vertices_world[index].z = object->vertices_local[index].z + object->world_pos.z;
        
    }
}

/**
* Convert the local coordinates into world and camera coordinates for shading
* and projection. Note the viewer is at (0,0,0) with angles 0,0,0
* so the transformation is imply to add the world position to each 
* local vertex
*/
void object_view_transformation(Object *object, Matrix *view_inverse) {  

        for(int index = 0; index < object->num_vertices; index++) 
        {
            object->vertices_camera[index] = *vector_matrix_mul(&object->vertices_world[index], view_inverse);
        }
}

/**
* Determines if object is out of frame by comparing bounding sphere to z and then x,y frame.
* @return 1 means object is out of frame and should be removed. 0 means it should not be removed.
*/
int object_culling(Object* object, Matrix* view_inverse, int mode) {
    // this function determines if an entire object is within the viewing volume 
    // or not by testing if the bounding sphere of the object in question 
    // is within the viewing volume. In essence, this function "culls" entire objects

    float x_bsphere,    // the x,y,z components of the projected center of object
          y_bsphere,
          z_bsphere,
          radius,       // the radius of object
          x_compare,    // the extents of the clipping volume in x and y at the 
          y_compare;    // bound spheres current z

    //first transform world position of object into camera coordinates
    Vector view_pos = *vector_matrix_mul(&object->world_pos, view_inverse);
    x_bsphere = view_pos.x;
    y_bsphere = view_pos.y;
    z_bsphere = view_pos.z;

    // extract radius of object
    radius = object->radius;

    if(mode == OBJECT_CULL_Z_MODE)
    {
        // first test against near and far z planes
        if(((z_bsphere - radius) > CLIP_FAR_Z) || ((z_bsphere + radius) < CLIP_NEAR_Z)) {
            return 1;
        } else {
            return 0;
        } // end if z only
    }
    else
    {
        // perform full x,y,z test
        if(((z_bsphere - radius) > CLIP_FAR_Z) || ((z_bsphere + radius) < CLIP_NEAR_Z)) {
            return 1;
        }

        // test against x right and left planes, first compute viewing volume
        // extents at position z position of bounding sphere

        x_compare = (( (float) WINDOW_WIDTH / 2) * z_bsphere) / VIEWING_DISTANCE;

        if(((x_bsphere - radius) > x_compare) || ((x_bsphere + radius) < (-x_compare))) {
            return 1;
        }


        // finally test against y top and bottom planes
        y_compare = (INVERSE_ASPECT_RATIO * ((float) WINDOW_HEIGHT / 2) * z_bsphere) / VIEWING_DISTANCE;

        if(((y_bsphere - radius) > y_compare) || ((y_bsphere + radius) < (-y_compare))) {
            return 1;
        }
        
        // else it cant be removed.
        return 0;
    }
}

void clip_object_3D(Object* object, int mode) {
    //this fnuction clip an object in camera coordiantes against the 3D viewing
    // volume. The function has 2 mode of operation. In CLIP_Z_MODE the 
    // function performs only a simple z extend clip with the near and far clipping
    // planes. In CLIP_XYZ_MODE the function performs a full 3D clip.

    int curr_poly;      // the current polygon being processed

    float x1, y1, z1,
          x2, y2, z2,
          x3, y3, z3,
          x4, y4,z4,    // working variables used to hold vertices

          x1_compare,   // used to hold clipping points on x and y
          y1_compare,
          x2_compare,
          y2_compare,
          x3_compare,
          y3_compare,
          x4_compare,
          y4_compare;

    // test if trivial z clipping is being requested
    if(mode == CLIP_Z_MODE)
    {
        // attempt to clip each polygon against viewing volume
        for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
        {
            // reset clipped variable
            // otherwise once clipped object will always be clipped.
            //object->polys[curr_poly].clipped = 0;

            // extract z components
            z1 = object->vertices_camera[object->polys[curr_poly].vertex_list[0]].z;
            z2 = object->vertices_camera[object->polys[curr_poly].vertex_list[1]].z;
            z3 = object->vertices_camera[object->polys[curr_poly].vertex_list[2]].z;

            // est if this is a quad
            if(object->polys[curr_poly].num_points == 4) 
            {
                // extract 4th z component
                z4 = object->vertices_camera[object->polys[curr_poly].vertex_list[3]].z;
            }
            else {
                z4 = z3;
            }

            // perfrom near and far z clipping test
            if( (z1 < CLIP_NEAR_Z && z2 < CLIP_NEAR_Z && z3 < CLIP_NEAR_Z && z4 < CLIP_NEAR_Z) ||
                (z1 > CLIP_FAR_Z && z2 > CLIP_FAR_Z && z3 > CLIP_FAR_Z && z4 > CLIP_FAR_Z))
            {
                // set clipped flag
                object->polys[curr_poly].clipped = 1;
            }
        } // end for curr_poly
    }
    else
    {
        // CLIP_XYZ_MODE, perform full 3D viewing volume clip
        for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
        {
            // reset clipped variable
            // otherwise once clipped object will always be clipped.
            object->polys[curr_poly].clipped = 0;

            // extract x,y and z components
            x1 = object->vertices_camera[object->polys[curr_poly].vertex_list[0]].x;
            y1 = object->vertices_camera[object->polys[curr_poly].vertex_list[0]].y;
            z1 = object->vertices_camera[object->polys[curr_poly].vertex_list[0]].z;

            x2 = object->vertices_camera[object->polys[curr_poly].vertex_list[1]].x;
            y2 = object->vertices_camera[object->polys[curr_poly].vertex_list[1]].y;
            z2 = object->vertices_camera[object->polys[curr_poly].vertex_list[1]].z;

            x3 = object->vertices_camera[object->polys[curr_poly].vertex_list[2]].x;
            y3 = object->vertices_camera[object->polys[curr_poly].vertex_list[2]].y;
            z3 = object->vertices_camera[object->polys[curr_poly].vertex_list[2]].z;

            // test if this is a quad
            if(object->polys[curr_poly].num_points == 4)
            {
                // extract 4th vertex
                x4 = object->vertices_camera[object->polys[curr_poly].vertex_list[3]].x;
                y4 = object->vertices_camera[object->polys[curr_poly].vertex_list[3]].y;
                z4 = object->vertices_camera[object->polys[curr_poly].vertex_list[3]].z;

                // do clipping
                if(!((z1 > CLIP_NEAR_Z || z2 > CLIP_NEAR_Z || z3 > CLIP_NEAR_Z || z4 > CLIP_NEAR_Z) &&
                     (z1 < CLIP_FAR_Z || z2 < CLIP_FAR_Z || z3 < CLIP_FAR_Z || z4 < CLIP_FAR_Z)))
                {
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }
                

                // pre-compute x comparison ranges
                x1_compare = (((float) WINDOW_WIDTH / 2) * z1) / VIEWING_DISTANCE;
                x2_compare = (((float) WINDOW_WIDTH / 2) * z2) / VIEWING_DISTANCE;
                x3_compare = (((float) WINDOW_WIDTH / 2) * z3) / VIEWING_DISTANCE;
                x4_compare = (((float) WINDOW_WIDTH / 2) * z4) / VIEWING_DISTANCE;

                // perform x tests
                if(!((x1 > (-x1_compare) || x2 > (-x2_compare) || x3 > (-x3_compare) || x4 > (-x4_compare)) &&
                     (x1 < x1_compare || x2 < x2_compare || x3 < x3_compare || x4 < x4_compare)))
                {
                    // set clipped flag
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }

                //pre-compute y comparison ranges
                y1_compare = (((float) WINDOW_HEIGHT / 2) * z1) / VIEWING_DISTANCE;  // inverse aspect ratio here?
                y2_compare = (((float) WINDOW_HEIGHT / 2) * z2) / VIEWING_DISTANCE; 
                y3_compare = (((float) WINDOW_HEIGHT / 2) * z3) / VIEWING_DISTANCE; 
                y4_compare = (((float) WINDOW_HEIGHT / 2) * z4) / VIEWING_DISTANCE; 

                // perform y test
                if(!((y1 > (-y1_compare) || y2 > (-y2_compare) || y3 > (-y3_compare) || y4 > (-y4_compare)) && 
                    (y1 < y1_compare || y2 < y2_compare || y3 < y3_compare || y4 < y4_compare)))
                {
                    // set clipped flag
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }         
            } // end if quad
            else
            {
                // must be triangle, perform clipping tests on only 3 vertices
                object->polys[curr_poly].clipped = 0;

                // do clipping tests

                // perform near and far clipping first
                if( (z1 < CLIP_NEAR_Z && z2 < CLIP_NEAR_Z && z3 < CLIP_NEAR_Z && z4 < CLIP_NEAR_Z) ||
                (z1 > CLIP_FAR_Z && z2 > CLIP_FAR_Z && z3 > CLIP_FAR_Z))
                {
                    // set clipped flag
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }

                // pre-compute x comparison ranges
                x1_compare = (((float) WINDOW_WIDTH / 2) * z1) / VIEWING_DISTANCE;
                x2_compare = (((float) WINDOW_WIDTH / 2) * z2) / VIEWING_DISTANCE;
                x3_compare = (((float) WINDOW_WIDTH / 2) * z3) / VIEWING_DISTANCE;

                // perform x tests
                if(!((x1 > (-x1_compare) || x2 > (-x2_compare) || x3 > (-x3_compare)) &&
                     (x1 < x1_compare || x2 < x2_compare || x3 < x3_compare)))
                {
                    // set clipped flag
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }

                //pre-compute y comparison ranges
                y1_compare = (((float) WINDOW_HEIGHT / 2) * z1) / VIEWING_DISTANCE;  // inverse aspect ratio here?
                y2_compare = (((float) WINDOW_HEIGHT / 2) * z2) / VIEWING_DISTANCE; 
                y3_compare = (((float) WINDOW_HEIGHT / 2) * z3) / VIEWING_DISTANCE; 

                // perform y test
                if(!((y1 > (-y1_compare) || y2 > (-y2_compare) || y3 > (-y3_compare)) && 
                    (y1 < y1_compare || y2 < y2_compare || y3 < y3_compare)))
                {
                    // set clipped flag
                    object->polys[curr_poly].clipped = 1;
                    continue;
                }
            } // end else triangle
        }
    }
}

// to reset list call (NULL, RESET_POLY_LIST)
void generate_poly_list(Object* object, int mode) {
    // this function is used to generate the final plygon list that will be
    // rendered. Object by object the list is built up.

    int vertex,
        curr_vertex,
        curr_poly;
    
    // test if this is the first object to be inserted
    if(mode == RESET_POLY_LIST)
    {
        // reset number of polys to 0.
        num_polys_frame = 0;
        return;
    } 

    for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
    {
        // insert all visible polygons into polygon list
        if(object->polys[curr_poly].visible && !object->polys[curr_poly].clipped)
        {
            // add this poly to poly list

            // first copy data and vertices into an open slot in storage area
            world_poly_storage[num_polys_frame].num_points = object->polys[curr_poly].num_points;
            world_poly_storage[num_polys_frame].color      = object->polys[curr_poly].color;
            world_poly_storage[num_polys_frame].shade      = object->polys[curr_poly].shade;
            world_poly_storage[num_polys_frame].two_sided  = object->polys[curr_poly].two_sided;
            world_poly_storage[num_polys_frame].visible    = object->polys[curr_poly].visible;
            world_poly_storage[num_polys_frame].clipped    = object->polys[curr_poly].clipped;
            world_poly_storage[num_polys_frame].active     = object->polys[curr_poly].active;

            // now copy vertices
            for(curr_vertex = 0; curr_vertex < object->polys[curr_poly].num_points; curr_vertex++)
            {
                // extract vertex number
                vertex = object->polys[curr_poly].vertex_list[curr_vertex];
                // extract x,y,z values
                world_poly_storage[num_polys_frame].vertex_list[curr_vertex].x = object->vertices_camera[vertex].x;
                world_poly_storage[num_polys_frame].vertex_list[curr_vertex].y = object->vertices_camera[vertex].y;
                world_poly_storage[num_polys_frame].vertex_list[curr_vertex].z = object->vertices_camera[vertex].z;
            }

            // assing pointer to it
            world_polys[num_polys_frame] = &world_poly_storage[num_polys_frame];

            // increment number of polys
            num_polys_frame++;

        } // end if poly visible
    } // end for curr_poly
}


/**
* Function handles case for when polygon is only partly inside view frustum
* with the other part being in behind camera (z < CLIP_NEAR_Z). If this happens then
* approach is to determine intersection of polygon with the camera plane and clip the 
* polygon to these new points.
* If polygon is a quad then simply cut it up into 2 triangles and redo the process. Similar
* thing can happen if triangle only has 1 point outside of frame. 
*/
void clip_polygon() {

    int curr_poly;     // polygon index

    // iterate through each polygon in render list
    for(curr_poly = 0; curr_poly < num_polys_frame; curr_poly++)
    {
        int num_verts_outside=0;        // counts how many points are outside.
        int vertex_ccodes[3] = {0,0,0}; // holds codes of which points that are outside.

        int v0, v1, v2; // indexes

        float t1, t2,
              xi, yi,
              x01i, y01i,
              x02i, y02i;

        Vector v, u;

        // polygon is skipped.
        if(world_polys[curr_poly]->clipped || (!world_polys[curr_poly]->visible)) {
            continue;
        }

        // polygon is quad, which means that we will now cut this into 2 new triangles, where
        // one of the polygon takes the place of the old one and the other is pushed.
        if(world_polys[curr_poly]->num_points == 4)
        {
            // if all points on quad is inside then simply continue.
            if((world_polys[curr_poly]->vertex_list[0].z > CLIP_NEAR_Z) &&
               (world_polys[curr_poly]->vertex_list[1].z > CLIP_NEAR_Z) &&
               (world_polys[curr_poly]->vertex_list[2].z > CLIP_NEAR_Z) &&
               (world_polys[curr_poly]->vertex_list[3].z > CLIP_NEAR_Z))
            {
                continue;
            }

            facet polygon;
            polygon.num_points  = 3;
            polygon.color       = world_polys[curr_poly]->color;
            polygon.shade       = world_polys[curr_poly]->shade;
            polygon.two_sided   = world_polys[curr_poly]->two_sided;
            polygon.visible     = world_polys[curr_poly]->visible;
            polygon.clipped     = world_polys[curr_poly]->clipped;
            polygon.active      = world_polys[curr_poly]->active;
            polygon.normal      = world_polys[curr_poly]->normal;
            
            // make the old polygon be vert 0,1,2 by simply reducing
            // number of points
            world_polys[curr_poly]->num_points = 3; 
            //world_polys[curr_poly]->shade = 0xFF0000FF;
             
            // new one will be 0,2,3;
            polygon.vertex_list[0].x = world_polys[curr_poly]->vertex_list[0].x;
            polygon.vertex_list[0].y = world_polys[curr_poly]->vertex_list[0].y;
            polygon.vertex_list[0].z = world_polys[curr_poly]->vertex_list[0].z;

            polygon.vertex_list[1].x = world_polys[curr_poly]->vertex_list[2].x;
            polygon.vertex_list[1].y = world_polys[curr_poly]->vertex_list[2].y;
            polygon.vertex_list[1].z = world_polys[curr_poly]->vertex_list[2].z;

            polygon.vertex_list[2].x = world_polys[curr_poly]->vertex_list[3].x;
            polygon.vertex_list[2].y = world_polys[curr_poly]->vertex_list[3].y;
            polygon.vertex_list[2].z = world_polys[curr_poly]->vertex_list[3].z;

            // add new polygon to pipeline
            world_poly_storage[num_polys_frame] = polygon;
            // assing pointer to it
            world_polys[num_polys_frame] = &world_poly_storage[num_polys_frame];
            // increment number of polys
            num_polys_frame++;
            
        } // end if polygon is quad.

        // if polygon was quad then current polygon will now be reduced and can be
        // passed into triangle for z-clipping.


        // polygon is triangle.
        if(world_polys[curr_poly]->num_points == 3)
        {
            // determine which points lie inside or outside of z_near

            // if z1 is behind near_z then set vertex code to 1
            if(world_polys[curr_poly]->vertex_list[0].z < CLIP_NEAR_Z) {
                vertex_ccodes[0] = 1;
                num_verts_outside++;
            }
            // if z2 is behind near_z then set vertex code to 1
            if(world_polys[curr_poly]->vertex_list[1].z < CLIP_NEAR_Z) {
                vertex_ccodes[1] = 1;
                num_verts_outside++;
            }
            // if z3 is behind near_z then set vertex code to 1
            if(world_polys[curr_poly]->vertex_list[2].z < CLIP_NEAR_Z) {
                vertex_ccodes[2] = 1;
                num_verts_outside++;
            }

            // case 1: only a single vertice is inside view frustum meaning that
            //         the 2 points outside can just be clipped to z_near;
            if(num_verts_outside == 2)
            {

                // find out which of the points is the one still inside (0 == inside)
                // temp variables automatically point back to working variables.
                // Thus, you would not have to directly know which one is which, since
                // it knows that itself.  
                if(vertex_ccodes[0] == 0)
                {
                    v0 = 0;
                    v1 = 1;
                    v2 = 2;
                }
                else
                if(vertex_ccodes[1] == 0)
                {
                    v0 = 1;
                    v1 = 2;
                    v2 = 0;
                }
                else 
                {
                    v0 = 2;
                    v1 = 0;
                    v2 = 1;
                };

                // step 2: clip each edge
                // basically we are going to generate
                // the parametric line + = v0 + v01*t
                // then solve for t when the z component 
                // is equal to CLIP_NEAR_Z, then plug that
                // back into to solve for x,y of the 3D line.

                // clip ege v0->v1
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);

                // the intersection occurs when z = CLIP_NEAR_Z, so t:
                t1 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug back in and find x,y interseciton with the plane
                xi = world_polys[curr_poly]->vertex_list[v0].x + v.x * t1;
                yi = world_polys[curr_poly]->vertex_list[v0].y + v.y * t1;

                // now overwrite the vertex with new vertex
                world_polys[curr_poly]->vertex_list[v1].x = xi;
                world_polys[curr_poly]->vertex_list[v1].y = yi;
                world_polys[curr_poly]->vertex_list[v1].z = CLIP_NEAR_Z;

                // clip edge v0->v2
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);

                // the intersection occurs when z = CLIP_NEAR_Z, so t:
                t2 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug t back in and find x,y intersection with the plane
                xi = world_polys[curr_poly]->vertex_list[v0].x + v.x * t2;
                yi = world_polys[curr_poly]->vertex_list[v0].y + v.y * t2;

                // now overwrite vertex with new vertex
                world_polys[curr_poly]->vertex_list[v2].x = xi;
                world_polys[curr_poly]->vertex_list[v2].y = yi;
                world_polys[curr_poly]->vertex_list[v2].z = CLIP_NEAR_Z;

                // re-compute normal
                u = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);
                world_polys[curr_poly]->normal = *vector_cross_product(&u, &v);

            } // end if 2 points outside
            else 
            // case 2: there are 2 points that lie inside the view frustum and
            //         only a single point inside. This case needs to construct a
            //         completely new extra polygon.
            if(num_verts_outside == 1)
            {
                // step 0: copy the polygon.
                facet temp_polygon;
                temp_polygon.num_points = 3;
                temp_polygon.color      = world_polys[curr_poly]->color;
                temp_polygon.shade      = world_polys[curr_poly]->shade;
                temp_polygon.two_sided  = world_polys[curr_poly]->two_sided;
                temp_polygon.visible    = world_polys[curr_poly]->visible;
                temp_polygon.clipped    = world_polys[curr_poly]->clipped;
                temp_polygon.active     = world_polys[curr_poly]->active;

                temp_polygon.vertex_list[0] = world_polys[curr_poly]->vertex_list[0];
                temp_polygon.vertex_list[1] = world_polys[curr_poly]->vertex_list[1];
                temp_polygon.vertex_list[2] = world_polys[curr_poly]->vertex_list[2];

                // step 1: find vertex index for exterior vertex
                // z1 is outside of view frustum
                if(vertex_ccodes[0] == 1)
                {
                    v0 = 0; v1 = 1; v2 = 2;
                }
                else 
                // z2 is outside of view frustum
                if(vertex_ccodes[1] == 1)
                {
                    v0 = 1; v1 = 2; v2 = 0;
                }
                // z3 is outside of view frustum
                else 
                {
                    v0 = 2; v1 = 0; v2 = 1;
                };

                // step 2: clip each edge
                // basically we are going to generate the
                // parametric line p = v0 + v01 * t
                // then solve for t when the z component is equal to
                // CLIP_NEAR_Z, then plug that 
                // back into to to solve for x,y of the 3D line.

                // clip edge v0->v1
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);

                // the intersection occurs when z = CLIP_NEAR_Z, so t:
                t1 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug t back in and ifnd x,y intersection with the plane
                x01i = world_polys[curr_poly]->vertex_list[v0].x + v.x * t1;
                y01i = world_polys[curr_poly]->vertex_list[v0].y + v.y * t1;

                // clip edge v0->v2
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);

                // the intersection occurs when z = CLIP_NEAR_Z, so t:
                t2 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug t back in and find x,y intersection with the plane
                x02i = world_polys[curr_poly]->vertex_list[v0].x + v.x * t2;
                y02i = world_polys[curr_poly]->vertex_list[v0].y + v.y * t2;

                // now we have both intersection points,
                // we must overwrite the inplace
                // polygon's vertex 0 with the intersection point,
                // this poly 1 of 2 from 
                // the split

                // now overwrite vertex with new vertex
                world_polys[curr_poly]->vertex_list[v0].x = x01i;
                world_polys[curr_poly]->vertex_list[v0].y = y01i;
                world_polys[curr_poly]->vertex_list[v0].z = CLIP_NEAR_Z;

                // now comes the hard part,
                // we have to carefully create a new polygon
                // from the 2 intersection points and v2,
                // this polygon will be inserted
                // at the end of the rendering list,
                // but for now, we are building it up
                // in temp_polygon

                // so leave v2 alone, but overwrite v1 with v01,
                // and overwrite v0 with v02

                temp_polygon.vertex_list[v1].x = x01i;
                temp_polygon.vertex_list[v1].y = y01i;
                temp_polygon.vertex_list[v1].z = CLIP_NEAR_Z;

                temp_polygon.vertex_list[v0].x = x02i;
                temp_polygon.vertex_list[v0].y = y02i;
                temp_polygon.vertex_list[v0].z = CLIP_NEAR_Z;

                // re-compute normals

                // poly 1 first, in place
                u = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);
                world_polys[curr_poly]->normal = *vector_cross_product(&u, &v);

                // now poly 2, temp_polygon
                u = *vector_sub(&temp_polygon.vertex_list[v0], &temp_polygon.vertex_list[v1]);
                v = *vector_sub(&temp_polygon.vertex_list[v0], &temp_polygon.vertex_list[v2]);
                temp_polygon.normal = *vector_cross_product(&u, &v);

                // add new polygon to pipeline
                world_poly_storage[num_polys_frame] = temp_polygon;
                // assing pointer to it
                world_polys[num_polys_frame] = &world_poly_storage[num_polys_frame];
                // increment number of polys
                num_polys_frame++;

            } // end else if num_verts_outside == 1
            else {
                continue;
            };

        } // end if num_points == 3.

    } // end for loop
}



int polygon_compare(facet** arg1, facet** arg2) {
    // this function compares the average z's of two polygons and is used by the
    // depth sort surface ordering algorithm

    float z1, z2;
    facet_ptr poly_1, poly_2;

    // dereference the poly pointers
    poly_1 = (facet*) *arg1;
    poly_2 = (facet*) *arg2;

    // compute z average of each polygon
    if(poly_1->num_points == 3)
    {
        // compute z average of each polygon
        z1 = (float) 0.33 * (poly_1->vertex_list[0].z + poly_1->vertex_list[1].z + poly_1->vertex_list[2].z);

    }
    else
    {
        z1 = (float) 0.25 * (poly_1->vertex_list[0].z + poly_1->vertex_list[1].z + poly_1->vertex_list[2].z + poly_1->vertex_list[3].z);
    }

    // now polygon 2

    if(poly_2->num_points == 3)
    {
        // compute average of 3 points polygon
        z2 = (float) 0.33 * (poly_2->vertex_list[0].z + poly_2->vertex_list[1].z + poly_2->vertex_list[2].z);
    }
    else 
    {
        // compute average of 4 point polygon
        z2 = (float) 0.25 * (poly_2->vertex_list[0].z + poly_2->vertex_list[1].z + poly_2->vertex_list[2].z + poly_2->vertex_list[3].z);
    }

    // compare z1 and z2, such that polys will be sorted in descending Z order.
    if(z1 > z2) 
    {
        return(-1);
    }
    else if(z1 < z2)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

void sort_polygon_list(void) {
    // this function des a simple z sort on the poly list to order surfaces
    // the list is sorted in descending order, i.e farther polygons first.
    qsort((void *) world_polys, num_polys_frame, sizeof(facet*), (const void*) polygon_compare);
}

/*
* Draws all polygons in list. Similar to object_draw_solid.
*/
void draw_poly_list(uint32_t *pixelmap) {
    // this function draws the golbal polygon list generated by calls to
    // generate_poly_list

    int curr_poly;      // the current polygon

    float x1, y1, z1,
          x2, y2, z2,
          x3, y3, z3,
          x4, y4, z4;

    // draw each polygon in list
    for(curr_poly = 0; curr_poly < num_polys_frame; curr_poly++)
    {
        int is_quad = 0;
        // do Z clipping first before projection
        z1 = world_polys[curr_poly]->vertex_list[0].z;
        z2 = world_polys[curr_poly]->vertex_list[1].z;
        z3 = world_polys[curr_poly]->vertex_list[2].z;
    

        // test if this is a quad
        if(world_polys[curr_poly]->num_points == 4)
        {
            // extract vertex number and z component for clipping and projection
            z4 = world_polys[curr_poly]->vertex_list[3].z;

            // set quad flag
            is_quad = 1;
        } else {
            z4 = z3;
        }

        #if 0
                // perform z clipping test

                if((z1<clip_near_z && z2 < clip_near_z && z3 < clip_near_z && z4 < clip_near_z) || 
                (z1 > clip_far_z && z2 > clip_far_z && z3 > clip_far_z && z4 > clip_far_z))
                {
                    continue;
                }
        #endif

        x1 = world_polys[curr_poly]->vertex_list[0].x;
        y1 = world_polys[curr_poly]->vertex_list[0].y;

        x2 = world_polys[curr_poly]->vertex_list[1].x;
        y2 = world_polys[curr_poly]->vertex_list[1].y;

        x3 = world_polys[curr_poly]->vertex_list[2].x;
        y3 = world_polys[curr_poly]->vertex_list[2].y;

        // compute screen position of points
        x1 = (((float) WINDOW_WIDTH / 2)  + x1 * VIEWING_DISTANCE / z1);
        y1 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y1 * VIEWING_DISTANCE / z1);

        x2 = (((float) WINDOW_WIDTH / 2)  + x2 * VIEWING_DISTANCE / z2);
        y2 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y2 * VIEWING_DISTANCE / z2);

        x3 = (((float) WINDOW_WIDTH / 2)  + x3 * VIEWING_DISTANCE / z3);
        y3 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y3 * VIEWING_DISTANCE / z3);

            

        //shade instead of color according to Lamotte.
        triangle_draw_2D((int) x1, (int) y1, (int) x2, (int) y2, (int) x3, (int) y3, world_polys[curr_poly]->shade, pixelmap);

        // draw second poly if this is a quad
        if(is_quad)
        {
            // extract the point
            x4 = world_polys[curr_poly]->vertex_list[3].x;
            y4 = world_polys[curr_poly]->vertex_list[3].y;

            x4 = (((float) WINDOW_WIDTH / 2)  + x4 * VIEWING_DISTANCE / z4);
            y4 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y4 * VIEWING_DISTANCE / z4);

            triangle_draw_2D((int) x1, (int) y1, (int) x3, (int) y3, (int) x4, (int) y4, world_polys[curr_poly]->shade, pixelmap);
        } // end if quad
    } // end for curr_poly
}

void draw_poly_list_z(uint32_t* pixelmap) {
    // this function draws the global polygon list generated by calls to 
    // generate_poly_list using the z buffer triangle system.

    int curr_poly;      // the current polygon

    float x1, y1, z1,
          x2, y2, z2,
          x3, y3, z3,
          x4, y4, z4;

    // draw each polygon in list
    for(curr_poly = 0; curr_poly < num_polys_frame; curr_poly++)
    {
        int is_quad = 0;

        // do Z clipping first before projection
        z1 = world_polys[curr_poly]->vertex_list[0].z;
        z2 = world_polys[curr_poly]->vertex_list[1].z;
        z3 = world_polys[curr_poly]->vertex_list[2].z;
    

        // test if this is a quad
        if(world_polys[curr_poly]->num_points == 4)
        {
            // extract vertex number and z component for clipping and projection
            z4 = world_polys[curr_poly]->vertex_list[3].z;

            // set quad flag
            is_quad = 1;
        } 
        else {
            z4 = z3;
        }

        // perform z clipping test
        if((z1 < CLIP_NEAR_Z && z2 < CLIP_NEAR_Z && z3 < CLIP_NEAR_Z && z4 < CLIP_NEAR_Z) || 
        (z1 > CLIP_FAR_Z && z2 > CLIP_FAR_Z && z3 > CLIP_FAR_Z && z4 > CLIP_FAR_Z))
        {
            continue;
        }

        x1 = world_polys[curr_poly]->vertex_list[0].x;
        y1 = world_polys[curr_poly]->vertex_list[0].y;

        x2 = world_polys[curr_poly]->vertex_list[1].x;
        y2 = world_polys[curr_poly]->vertex_list[1].y;

        x3 = world_polys[curr_poly]->vertex_list[2].x;
        y3 = world_polys[curr_poly]->vertex_list[2].y;

        // compute screen position of points
        x1 = (((float) WINDOW_WIDTH / 2)  + x1 * VIEWING_DISTANCE / z1);
        y1 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y1 * VIEWING_DISTANCE / z1);

        x2 = (((float) WINDOW_WIDTH / 2)  + x2 * VIEWING_DISTANCE / z2);
        y2 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y2 * VIEWING_DISTANCE / z2);

        x3 = (((float) WINDOW_WIDTH / 2)  + x3 * VIEWING_DISTANCE / z3);
        y3 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y3 * VIEWING_DISTANCE / z3);  

        //shade instead of color according to Lamotte.
        draw_triangle_3D_z((int) x1, (int) y1, (int) z1, (int) x2, (int) y2, (int) z2,(int) x3, (int) y3, (int) z3, world_polys[curr_poly]->shade, pixelmap);

        // draw second poly if this is a quad
        if(is_quad)
        {
            // extract the point
            x4 = world_polys[curr_poly]->vertex_list[3].x;
            y4 = world_polys[curr_poly]->vertex_list[3].y;

            x4 = (((float) WINDOW_WIDTH / 2)  + x4 * VIEWING_DISTANCE / z4);
            y4 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y4 * VIEWING_DISTANCE / z4);
        
            draw_triangle_3D_z((int) x1, (int) y1, (int) z1,(int) x3, (int) y3, (int) z3,(int) x4, (int) y4, (int) z4, world_polys[curr_poly]->shade, pixelmap);
        } // end if quad
    } // end for curr_poly
}


void fill_z_buffer(int value) {
    // this function fills the entire z buffer both banks with the sent value
    /*
    __asm__
        {
            ; bank 1

            les di,z_bank       ; point es:di to z buffer bank
            mov ax,value        ; move the value into ax
            mov cx,z_bank_size  ; number of bytes to fill
            shr cx,1            ; convert to number of words
            rep stosw           ; move the value into z buffer

        } // end asm
    */
    for(int i = 0; i < ALL_PIXELS; i++)
    {
        z_buffer[i] = value;
    }
    
}

void draw_tb_triangle_3d_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color, uint32_t* pixelmap) 
{
    // this function draws a triangle that has a flat top
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
    if(y1 == y2)
    {
        //perform computations for a triangle with a flat top
        if(x2 < x1)
        {
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
    else 
    {
        // bottom must be flat
        // test order of x3 and x2, note y2 == y3
        if(x3 < x2)
        {
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
    if(y1 < poly_clip_min_y) 
    {
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

    // start z buffer at proper bank
    //z_buffer = z_bank + (y1 << 8) + (y1 << 6);

    // test if x clipping is needed
    if(x1 >= poly_clip_min_x && x1 <= poly_clip_max_x &&
       x2 >= poly_clip_min_x && x2 <= poly_clip_max_x &&
       x3 >= poly_clip_min_x && x3 <= poly_clip_max_x)
    {
        // draw the triangle
        for(y_index = y1; y_index <= y3; y_index++)
        {
            // 16 bit fixed math point for the horizontal interpolation 
            // (can be avoided by setting z_middle to float).
            // z_middle set to float

            z_middle = z_left;
            bx = (z_right - z_left) / (1 + xe - xs);

            for(x_index = (int) xs; x_index <= (int) xe; x_index++)
            {
                //y * WINDOW_WIDTH) + x
                //printf("x_index_loop\n");
                //printf("x_index draw 1523: z_buffer[%d] = %d\n", y_index * WINDOW_WIDTH + x_index, z_buffer[y_index * WINDOW_WIDTH + x_index]);
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index])
                {
                    // update z buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int) z_middle;
                    // write to image buffer
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

            // adjust video and z buffer offset
            //z_buffer += 320;
            
        } // end for 

    } // end if no x clipping needed
    else
    {
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

            //printf("z_left: %f, z_right: %f, z_middle: %f\n", z_left, z_right, z_middle);

            // adjust starting point and ending point
            xs += dx_left;
            xe += dx_right;

            // adjust vertical z interpolants
            z_left += b1y;
            z_right += b2y;

            // clip line
            if(xs_clip < poly_clip_min_x)
            {
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
                if(z_middle < z_buffer[y_index * WINDOW_WIDTH + x_index])
                {
                    // update z buffer
                    z_buffer[y_index * WINDOW_WIDTH + x_index] = (int) z_middle;

                    // write to image buffer
                    display_draw_pixel(pixelmap, x_index, y_index, color);

                } // end if update z buffer

                // update current z value
                z_middle += bx;

            } // end draw z buffered line

            // adjust video and z_buffer offsets
            //z_buffer += 320;

        } // end for y_index

    } // ned else x clipping needed
}

void draw_triangle_3D_z(int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int x3, int y3, int z3,
                        int color, uint32_t* pixelmap) 
{
    // this function sorts the vertices, and splits the triangles into 2 halves and draws them.

    int temp_x,     // used for sorting
        temp_y,
        temp_z,
        new_x,      // used to compute new x and z at triangle splitting point
        new_z;

    // test for h lines and v lines
    if((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3)) {
        return;
    }

    // sort p1, p2, p3 in ascending y order
    if(y2 < y1)
    {
        temp_x = x2;
        temp_y = y2;
        temp_z = z2;

        x2 = x1;
        y2 = y1;
        z2 = z1;

        x1 = temp_x;
        y1 = temp_y;
        z1 = temp_z;
    }

    // now we know that p1 and p2 are in order
    if(y3 < y1)
    {
        temp_x = x3;
        temp_y = y3;
        temp_z = z3;

        x3 = x1;
        y3 = y1;
        z3 = z1;

        x1 = temp_x;
        y1 = temp_y;
        z1 = temp_z;
    }

    // finally test y3 against y2
    if(y3 < y2)
    {
        temp_x = x3;
        temp_y = y3;
        temp_z = z3;

        x3 = x2;
        y3 = y2;
        z3 = z2;

        x2 = temp_x;
        y2 = temp_y;
        z2 = temp_z;
    }

    // do trivial rejection tests
    if(y3 < poly_clip_min_y || y1 > poly_clip_max_y ||
       (x1 < poly_clip_min_x && x2 < poly_clip_min_x && x3 < poly_clip_min_x) ||
       (x1 > poly_clip_max_x && x2 > poly_clip_max_x && x3 > poly_clip_max_x))
    {
        return;
    }

    // test if top of triangle is flat
    if(y1 == y2 || y2 == y3)
    {
        draw_tb_triangle_3d_z(x1,y1,z1,x2,y2,z2,x3,y3,z3,color,pixelmap);
    }
    else 
    {
        // general tirangle that needs to be borken up along long edge
        // compute new x,z at split point

        new_x = x1 + (int)((float)(y2 - y1) * (float)(x3 - x1) / (float)(y3 - y1));
        new_z = z1 + (int)((float)(y2 - y1) * (float)(z3 - z1) / (float)(y3 - y1));

        // draw each sub-triangle
        if(y2 >= poly_clip_min_y && y1 < poly_clip_max_y)
        {
            draw_tb_triangle_3d_z(x1,y1,z1,new_x,y2,new_z,x2,y2,z2,color,pixelmap);
        }
        if(y3 >= poly_clip_min_y && y1 < poly_clip_max_y)
        {
            draw_tb_triangle_3d_z(x2,y2,z2,new_x,y2,new_z,x3,y3,z3,color,pixelmap);
        }
    }
}

void draw_triangle_2d_gouraud(int x1, int y1, int x2, int y2, int x3, int y3, int color, int intensity_1, int intensity_2, int intensity_3, uint32_t *pixelmap) {
    // this function draws a Gouraud shaded triangle but does not perform an clipping 
    // and is very slow

    int x,y,            // looping variables
        x_index,        // integer texture coordinates
        y_index,
        bottom_1,       // distance from top to middle of triangle on y axis
        bottom_2;       // distance from middle to bottom triangle on y axis
    
    float dx_right,     // the dx/dy ratio of the right edge of line
          dx_left,      // the dx/dy ratio of the left edge of line
          xs,xe,        // the starting and ending points of the edges
          height_left,  // the heights of the triangle
          height_right;

    float intensity_right,  // the intensity of the right edge of the triangle
          intensity_left,   // the intensity of the left edge of the triangle
          intensity_mid,    // the average between the right and left
          delta_y21, delta_y31; // the y delta's

    // compute height of sub triangles
    height_left = y2 - y1;
    height_right = y3 - y1;

    // compute distances from starting y vertex
    if(y2 > y3)
    {
        bottom_1 = y3;
        bottom_2 = y2;
    }
    else
    {
        bottom_1 = y2;
        bottom_2 = y3;
    }

    // compute edge deltas
    dx_left = (float) (x2 - x1) / height_left;
    dx_right = (float) (x3 - x1) / height_right;

    // set starting points
    xs = (float) x1;
    xe = (float) x1 + (float) 0.5;

    // compute shading constants
    delta_y21 = (float) 1 / (float) (y2 - y1);
    delta_y31 = (float) 1 / (float) (y3 - y1);

    for(y = y1; y <= bottom_1; y++)
    {
        // compute left and right edge intensities as a function of y
        intensity_left = delta_y21 * (float)((y2 - y) * intensity_1 + (y - y1) * intensity_2);
        intensity_right = delta_y31 * (float)((y3 - y) * intensity_1 + (y - y1) * intensity_3);
        
        // draw line
        for(x = (int) xs; x <= (int) xe; x++)
        {
            // compute x axis intensity interpolant
            intensity_mid = ((xe - x) * intensity_left + (x - xs) * intensity_right) / (xe - xs);

            // Mask to extract each color channel
            int alpha =  color & 0xFF000000,
                blue  = (color & 0x00FF0000) >> 16,
                green = (color & 0x0000FF00) >> 8,
                red   =  color & 0x000000FF;

            // Calculate the sum of the RGB values
            int sum = red + green + blue;

            // Avoid division by zero
            if (sum == 0) {
                sum = 1;
            }

            // value of new color based on intensity 0-255.
            int value = 4 * intensity_mid;

            // Compute the new color channels based on the ratio
            float percentage_red   = (float) red / sum,
                percentage_green = (float) green / sum,
                percentage_blue  = (float) blue / sum;

            red = percentage_red * value;
            green = percentage_green * value;
            blue = percentage_blue * value;

            // Combine the channels back into a single integer
            int new_color = alpha | (blue << 16) | (green << 8) | red;

            display_draw_pixel(pixelmap, x, y, new_color);

        } // end for x

        xs += dx_left;
        xe += dx_right;

    } // end for 


    // now recompute slope of shorter edge to make it complete triangle
    if(y3 > y2)
    {
        // recompute left edge slope
        dx_left = (float)(x3 - x2) / (float)(y3 - y2);
    }
    else 
    {
        // y2 > y3, recompute right edge slope
        dx_right = (float) (x2 - x3) / (float) (y2 - y3);
    }

    // draw remainder of triangle
    for(y--; y <= bottom_2; y++)
    {
        // compute left and right edge of intensities as a function of y
        intensity_left = (float)((y3 - y) * intensity_2 + (y - y2) * intensity_3) / (float)(y3 - y2);
        intensity_right = delta_y31 * (float) (y3 - y) * intensity_1 + (y - y2) * intensity_3;

        // draw line
        for(x = (int) xs; x <= (int) xe; x++)
        {
            // compute x axis intensity interpolant
            intensity_mid = ((xe - x)*intensity_left + (x - xs)*intensity_right) / (xe - xs);

             // Mask to extract each color channel
            int alpha =  color & 0xFF000000,
                blue  = (color & 0x00FF0000) >> 16,
                green = (color & 0x0000FF00) >> 8,
                red   =  color & 0x000000FF;

            // Calculate the sum of the RGB values
            int sum = red + green + blue;

            // Avoid division by zero
            if (sum == 0) {
                sum = 1;
            }

            // value of new color based on intensity 0-255.
            int value = 4 * intensity_mid;

            // Compute the new color channels based on the ratio
            float percentage_red   = (float) red / sum,
                percentage_green = (float) green / sum,
                percentage_blue  = (float) blue / sum;

            red = percentage_red * value;
            green = percentage_green * value;
            blue = percentage_blue * value;

            // Combine the channels back into a single integer
            int new_color = alpha | (blue << 16) | (green << 8) | red;

            display_draw_pixel(pixelmap, x, y, new_color);

        }

        xs += dx_left;
        xe += dx_right;
    }

}



