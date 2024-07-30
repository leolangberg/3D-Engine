#include "polygon.h"
#include "matrix.h"
#include "vector.h"
#include "../integration/display.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define poly_clip_min_y 0
#define poly_clip_max_y (216 - 1)
#define poly_clip_min_x 0
#define poly_clip_max_x (384 - 1)

Vector light_source = {-0.913913, 0.389759, -0.113369};
float ambient_light = 6;
Vector view_point = {0, 0, 0};

#define FLAT_SHADING 1

#define SHADE_GREY 31           // hex value = 1F
#define SHADE_GREEN 111         // hex value = 6F
#define SHADE_BLUE 159          // hex value = 9F
#define SHADE_RED 47            // hex value = 2F
#define SHADE_YELLOW 79         // hex value = 4F
#define SHADE_BROWN 223         // hex value = DF
#define SHADE_LIGHT_BROWN 207   // hex value = CF
#define SHADE_PURPLE 175        // hex value = AF
#define SHADE_CYAN 127          // hex value = 7F
#define SHADE_LAVENDER 191      // hex value = BF


Triangle triangle_create(const Vector* v1, const Vector* v2, const Vector* v3, int color) {
    Triangle triangle;
    triangle.v[0] = *v1;
    triangle.v[1] = *v2;
    triangle.v[2] = *v3;
    triangle.color = color;
    return triangle;
}


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

char *PLG_Get_Line(char *string, int max_length, FILE *fp) {
    // this function get a line from a PLG file and strips comments
    // just pretend it's a black box!

    char buffer[80];

    int length,     // length of line read
        index=0,    // looping variables
        index_2=0,
        parsed=0;   // has the current input line been parsed

    // get the next line of input, make sure there is something on the line

    while(1)
    {
        // get the line
        if(!fgets(buffer, max_length, fp)) {
            return(NULL);
        }

        // get length of line
        length = strlen(buffer);

        // kill the carriage return
        buffer[length-1] = 0;

        // reset index
        index = 0;

        //eat leading white space
        while(buffer[index]==' ') {
            index++;
        }

        // read the line into buffer, if "#" arrives in data stream then disregard
        // rest of line.
        parsed = 0;
        index_2 = 0;

        while(!parsed)
        {
            if(buffer[index]!='#' && buffer[index]!=';')
            {
                // insert character into output string
                string[index_2] = buffer[index];

                // test if this is a null terminator
                if(string[index_2] == 0) {
                    parsed = 1;
                }

                // move to next character
                index++;
                index_2++;
            }
            else
            {
                // insert a null termination since this is the end of the 
                // string for all intense purposes
                string[index_2] = 0;
                parsed = 1;
        
            }
        }

        // make sure we got a string and not a blank line
        if(strlen(string)) {
            return(string);
        }
    }
}

int PLG_Load_Object(Object* object, char *filename, float scale) {
    // this function loads an object off disk and allows it to be scaled.

    FILE *fp; // disk file
    static int id_number = 0; // used to set object id's
    char buffer[80],          // holds input string
         object_name[32],     // name of 3D object
         *token;              // current parsing token

    unsigned int total_vertices,    // total vertices in object
                 total_polys,       // total polygons per object
                 num_vertices,      // number of vertices on a polygon
                 color_des,         // the color descriptor of a polygon
                 logical_color,     // the final color of polygon
                 shading,           // the type of shading used on polygon
                 index,             // looping variables
                 index_2,
                 vertex_num,        // vertex number
                 vertex_0,
                 vertex_1,
                 vertex_2;
    
    float x,y,z;                    // a single vertex

    Vector u,v,normal;              // working vectors

    // open the disk file
    if((fp=fopen(filename, "r")) == NULL) {
        printf("Could not open file %s\n", filename);
        return 0;
    }

    // first we are looking for the header line that has the object name and
    // the number of vertices and polygons

    if(!PLG_Get_Line(buffer, 80, fp)) {
        printf("Error with PLG file %s (stop 1)", filename);
        fclose(fp);
        return 0;
    }

    // extract object name and number of vertices and polygons
    sscanf(buffer, "%s %d %d",object_name, &total_vertices, &total_polys);

    // set proper fields in object
    object->num_vertices = total_vertices;
    object->num_polys = total_polys;
    object->state = 1;

    printf("total_vertices: %d, total_polys: %d\n", total_vertices, total_polys);
    
    object->world_pos.x = 0;
    object->world_pos.y = 0;
    object->world_pos.z = 0;

    // set id number, maybe later also add the name of object in the structure?
    object->id = id_number++;


    // based on number of vertices, read vertex list into object
    for(index = 0; index < total_vertices; index++)
    {
        // read in vertex
        if(!PLG_Get_Line(buffer, 80, fp)) {
            printf("Error with PLG file %s (stop 2)", filename);
            fclose(fp);
            return 0;
        }

        sscanf(buffer,"%f %f %f", &x, &y, &z);

        // insert vertex into object
        object->vertices_local[index].x = x * scale;
        object->vertices_local[index].y = y * scale;
        object->vertices_local[index].z = z * scale;

        printf("x: %.2f, y: %.2f, z: %.2f\n", x, y , z);

    }

    // now read in polygon list
    for(index = 0; index < total_polys; index++)
    {
        // read in color and number of vertices for next polygon
        if(!PLG_Get_Line(buffer, 80, fp)) {
            printf("Error with PLG file %s (stop 3)", filename);
            fclose(fp);
            return 0;
        }

        // initialize token getter and get first token which is color descriptor
        if(!(token = strtok(buffer, " "))) {
            printf("Error with PLG file %s (stop 4)", filename);
            fclose(fp);
            return 0;
        }

        // test if number is hexadecimal

        if(token[0] == '0' && (token[1] == 'x' || token[1] == 'X')) {
            sscanf(&token[2], "%x", &color_des);
            // end if hex color specifier
            printf("if hex color desc\n");
        }
        else {
            color_des = atoi(token);
            // end if decimal
            printf("else decimal color desc\n");
        }
        

        // extract base color and type of shading
        logical_color = color_des & 0x00ff;
        shading       = color_des >> 12;

        printf("colordesc: %x, logical_color: %x, shading: %x\n", color_des, logical_color, shading);

        // read number of vertices in polygon
        if(!(token = strtok(NULL, " "))) {
            printf("Error with PLG file %s (stop 5)", filename);
            fclose(fp);
            return 0;
        }

        if((num_vertices = atoi(token)) <= 0) {
            printf("Error with PLG file %s (stop 6)", filename);
            fclose(fp);
            return 0;
        }

        // set fields in polygon structure

        object->polys[index].num_points = num_vertices;
        object->polys[index].color      = logical_color;
        object->polys[index].shading    = shading;
        object->polys[index].two_sided  = 0;
        object->polys[index].visible    = 1;
        object->polys[index].clipped    = 0;
        object->polys[index].active     = 1;

        printf("num_points: %d\n", num_vertices);

        // now read in polygon vertice list
        for(index_2 = 0; index_2 < num_vertices; index_2++)
        {
            printf("token: %s\n", token);
            // read in next vertex number
            if(!(token = strtok(NULL, " ")))
            {
                printf("Error with PLG file %s (stop 7)", filename);
                fclose(fp);
                return 0;
            }

            vertex_num = atoi(token);
            printf("vertex_num: %d\n", vertex_num);

            // insert vertex number into polygon
            object->polys[index].vertex_list[index_2] = vertex_num;
        }

        // compute length of the two co-planar edges of the polygon, since they
        // will be used in the computation of the dot-product later

        vertex_0 = object->polys[index].vertex_list[0];
        vertex_1 = object->polys[index].vertex_list[1];
        vertex_2 = object->polys[index].vertex_list[2];

        // the vector u = v0->v1
        u = *vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_1]);
        // the vector v = v0->v2
        v = *vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_2]);

        normal = *vector_cross_product(&v, &u);
        object->polys[index].normal = normal;
    }

    // close file
    fclose(fp);

    // compute object radius
    object->radius = compute_object_radius(object);

    return 1;
}

void remove_backfaces_and_shade(Object* object) {
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
        // compute the two vectors on polygon that have the same initial points
        vertex_0 = object->polys[curr_poly].vertex_list[0];
        vertex_1 = object->polys[curr_poly].vertex_list[1];
        vertex_2 = object->polys[curr_poly].vertex_list[2];

        // the vector u = v0->v1
        u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);

        // the vector v = v0->v2
        v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);

        // compute the normal to polygon v x u
        normal = *vector_cross_product(&v, &u);

        // compute the line of sight vector, since all coordinates are world all
        // object vertices are already relative to (0,0,0) thus

        /*
        * view_point mentioned but not declared? set to 0 for now.
        */
        sight.x = view_point.x - object->vertices_world[vertex_0].x;
        sight.y = view_point.y - object->vertices_world[vertex_0].y;
        sight.z = view_point.z - object->vertices_world[vertex_0].z;

        // compute the dot product between line of sight vector and normal to surface
        dp = vector_dot_product(&normal, &sight);

        // set the clip flagged appropriately
        if(dp>0)
        {
            // set visibility
            object->polys[curr_poly].visible = 1;

            // compute light intensity if needed
            if(object->polys[curr_poly].shading==FLAT_SHADING)
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

                    object->polys[curr_poly].shade = object->polys[curr_poly].color - (int) intensity;

                    // printf("\nintensity of polygon %d is %f", curr_poly, intensity);
            
                } // end if light is reflecting off surface
                else
                {
                    object->polys[curr_poly].shade = object->polys[curr_poly].color - (int) ambient_light;
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
    } // end for curr_poly
}

void draw_object_solid(Object* object, uint32_t* pixelmap) {
    // this function draws an object shaded solid and can perform
    // simple z extent clipping.
    // Z-clipping refers to clipping of object that are to far away or
    // behind camera.


    int curr_poly,          // the current polygon
        vertex_1,           // vertex index numbers
        vertex_2,
        vertex_3,
        vertex_4,
        is_quad=0;          // qadrilateral flag

    float x1,y1,z1,         // working variables
          x2,y2,z2,
          x3,y3,z3,
          x4,y4,z4;

    // compute position of object in world
    for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
    {
        // is this polygon visible?
        if(object->polys[curr_poly].visible == 0 || object->polys[curr_poly].clipped) {
            //printf("poly with index: %d is invisible or clipped\n", curr_poly);
            continue;
        }

        if(object->polys[curr_poly].visible == 1 || (object->polys[curr_poly].clipped == 0)) {
            //printf("poly with index: %d is visible\n", curr_poly);
            int i1 = object->polys[curr_poly].vertex_list[0];
            int i2 = object->polys[curr_poly].vertex_list[1];
            int i3 = object->polys[curr_poly].vertex_list[2];
            //printf("vertices: %d, %d, %d\n", object->polys[curr_poly].vertex_list[0], object->polys[curr_poly].vertex_list[1], object->polys[curr_poly].vertex_list[2]);

            //vector_print(&object->vertices_local[i1]);
            //vector_print(&object->vertices_local[i2]);
            //vector_print(&object->vertices_local[i3]);
            
            //printf("color: %x shade: %x\n", object->polys[curr_poly].color, object->polys[curr_poly].shade);

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

        #if 0
            // perform z clipping test

            if((z1<clip_near_z && z2 < clip_near_z && z3 < clip_near_z && z4 < clip_near_z) || 
            (z1 > clip_far_z && z2 > clip_far_z && z3 > clip_far_z && z4 > clip_far_z))
            {
                continue;
            }
        #endif
        
        // extract points of polygon
        x1 = object->vertices_camera[vertex_1].x;
        y1 = object->vertices_camera[vertex_1].y;

        x2 = object->vertices_camera[vertex_2].x;
        y2 = object->vertices_camera[vertex_2].y;

        x3 = object->vertices_camera[vertex_3].x;
        y3 = object->vertices_camera[vertex_3].y;

        float viewing_distance = 250;
        float aspect_ratio = 1;
        

        x1 = ((384.0f / 2) + x1 * viewing_distance / z1);
        y1 = ((216.0f / 2) + aspect_ratio * y1 * viewing_distance / z1);

        x2 = ((384.0f / 2) + x2 * viewing_distance / z2);
        y2 = ((216.0f / 2) + aspect_ratio * y2 * viewing_distance / z2);

        x3 = ((384.0f / 2) + x3 * viewing_distance / z3);
        y3 = ((216.0f / 2) + aspect_ratio * y3 * viewing_distance / z3);

        

        //printf("before draw function call\n");
        //printf("color: %x shade: %x\n", object->polys[curr_poly].color, object->polys[curr_poly].shade);
        
        //shade instead of color according to Lamotte.
        triangle_draw_2D((int) x1, (int) y1, (int) x2, (int) y2, (int) x3, (int) y3, object->polys[curr_poly].shade, pixelmap);

        // draw second poly if this is a quad
        if(is_quad)
        {
            // extract the point
            x4 = object->vertices_camera[vertex_4].x;
            y4 = object->vertices_camera[vertex_4].y;

            x4 = ((384.0f / 2) + x4 * viewing_distance / z4);
            y4 = ((216.0f / 2) + aspect_ratio * y4 * viewing_distance / z4);

            triangle_draw_2D((int) x1, (int) y1, (int) x3, (int) y3, (int) x4, (int) y4, object->polys[curr_poly].shade, pixelmap);
        } // end if quad
    } // end for curr_poly
}

void object_position(Object* object, int x, int y, int z) {
    object->world_pos.x = x;
    object->world_pos.y = y;
    object->world_pos.z = z;

}

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