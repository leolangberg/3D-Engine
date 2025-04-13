#include "polygon.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* For polygons that are two_sided we create duplicate mirrored polygons */
void mirror_two_sided_polygons(Object *object) {
    
    int vertex_0, vertex_1, vertex_2;
    Vector u,v, normal;

    for(int curr_poly = 0; curr_poly < object->num_polys; curr_poly++) {
        if(object->polys[curr_poly].two_sided == TWO_SIDED) {

            object->polys[curr_poly].two_sided = ONE_SIDED;
            
            object->polys[object->num_polys].num_points = object->polys[curr_poly].num_points;
            object->polys[object->num_polys].color      = object->polys[curr_poly].color;
            object->polys[object->num_polys].two_sided  = ONE_SIDED;
            object->polys[object->num_polys].visible    = object->polys[object->num_polys].visible;
            object->polys[object->num_polys].clipped    = object->polys[curr_poly].clipped;
            object->polys[object->num_polys].active     = object->polys[curr_poly].active;

            if(object->polys[curr_poly].num_points == 3) {
                object->polys[object->num_polys].vertex_list[0] = object->polys[curr_poly].vertex_list[1];
                object->polys[object->num_polys].vertex_list[1] = object->polys[curr_poly].vertex_list[0];
                object->polys[object->num_polys].vertex_list[2] = object->polys[curr_poly].vertex_list[2];
                object->num_vertices += 3;
            }
            else { // Quad
                object->polys[object->num_polys].vertex_list[0] = object->polys[curr_poly].vertex_list[1];
                object->polys[object->num_polys].vertex_list[1] = object->polys[curr_poly].vertex_list[0];
                object->polys[object->num_polys].vertex_list[2] = object->polys[curr_poly].vertex_list[3];
                object->polys[object->num_polys].vertex_list[3] = object->polys[curr_poly].vertex_list[2];
                object->num_vertices += 4;
            }


            vertex_0 = object->polys[object->num_polys].vertex_list[0];
            vertex_1 = object->polys[object->num_polys].vertex_list[1];
            vertex_2 = object->polys[object->num_polys].vertex_list[2];
            // the vector u = v0->v1
            u = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_1]);
            // the vector v = v0->v2
            v = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_2]);
    
            normal = vector_cross_product(&v, &u);
            object->polys[object->num_polys].normal = normal;

            object->num_polys++;
            
        }
    }
}


// this function is used to generate the final plygon list that will be
// rendered. Object by object the list is built up.
void generate_poly_list(facet *world_poly_storage, facet **world_polys, int *num_polys_frame, Object* object) {
    int vertex, curr_vertex, curr_poly;
    int p_num_polys_frame = *num_polys_frame; 

    // insert all visible polygons into polygon list
    for(curr_poly = 0; curr_poly < object->num_polys; curr_poly++) {
        if(object->polys[curr_poly].visible && !object->polys[curr_poly].clipped) {
            // first copy data and vertices into an open slot in storage area


            world_poly_storage[p_num_polys_frame].num_points = object->polys[curr_poly].num_points;
            world_poly_storage[p_num_polys_frame].color      = object->polys[curr_poly].color;
            world_poly_storage[p_num_polys_frame].shade[0]      = object->polys[curr_poly].shade[0];
            world_poly_storage[p_num_polys_frame].shade[1]      = object->polys[curr_poly].shade[1];
            world_poly_storage[p_num_polys_frame].shade[2]      = object->polys[curr_poly].shade[2];
            world_poly_storage[p_num_polys_frame].shade[3]      = object->polys[curr_poly].shade[3];
            world_poly_storage[p_num_polys_frame].two_sided  = object->polys[curr_poly].two_sided;
            world_poly_storage[p_num_polys_frame].visible    = object->polys[curr_poly].visible;
            world_poly_storage[p_num_polys_frame].clipped    = object->polys[curr_poly].clipped;
            world_poly_storage[p_num_polys_frame].active     = object->polys[curr_poly].active;

            // continue and copy vertices
            for(curr_vertex = 0; curr_vertex < object->polys[curr_poly].num_points; curr_vertex++) {
                // extract vertex number and then x,y,z values
                vertex = object->polys[curr_poly].vertex_list[curr_vertex];
                world_poly_storage[p_num_polys_frame].vertex_list[curr_vertex].x = object->vertices_camera[vertex].x;
                world_poly_storage[p_num_polys_frame].vertex_list[curr_vertex].y = object->vertices_camera[vertex].y;
                world_poly_storage[p_num_polys_frame].vertex_list[curr_vertex].z = object->vertices_camera[vertex].z;
            }

            // assing pointer to frame and increase number of polys.
            world_polys[p_num_polys_frame] = &world_poly_storage[p_num_polys_frame];
            *num_polys_frame += 1;
            p_num_polys_frame++;

        } // end if poly visible
    } // end for curr_poly
}

    // this function draws the global polygon list generated by calls to 
    // generate_poly_list using the z buffer triangle system.
void draw_poly_list_z(facet **world_polys, int *num_polys_frame, uint32_t* pixelmap, int *z_buffer) {
    float x1, y1, z1, x2, y2, z2,
          x3, y3, z3, x4, y4, z4;

    // draw each polygon in list
    for(int curr_poly = 0; curr_poly < *num_polys_frame; curr_poly++) {
        // do Z clipping first before projection
        z1 = world_polys[curr_poly]->vertex_list[0].z;
        z2 = world_polys[curr_poly]->vertex_list[1].z;
        z3 = world_polys[curr_poly]->vertex_list[2].z; 
        int is_quad = 0;

        // test if this is a quad
        // extract vertex number and z component for clipping and projection
        if(world_polys[curr_poly]->num_points == 4) {
            z4 = world_polys[curr_poly]->vertex_list[3].z;
            is_quad = 1;
        } else {
            z4 = z3;
        }

        // perform z clipping test
        if((z1 < CLIP_NEAR_Z && z2 < CLIP_NEAR_Z && z3 < CLIP_NEAR_Z && z4 < CLIP_NEAR_Z) || 
           (z1 > CLIP_FAR_Z && z2 > CLIP_FAR_Z && z3 > CLIP_FAR_Z && z4 > CLIP_FAR_Z))
        { continue; }

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
        draw_triangle_3D_z((int) x1, (int) y1, (int) z1, (int) x2, (int) y2, (int) z2,(int) x3, (int) y3, (int) z3, 
                            world_polys[curr_poly]->shade, pixelmap, z_buffer, FLAT_SHADING);

        // draw second poly if this is a quad
        if(is_quad) {
            // extract the point
            x4 = world_polys[curr_poly]->vertex_list[3].x;
            y4 = world_polys[curr_poly]->vertex_list[3].y;
            x4 = (((float) WINDOW_WIDTH / 2)  + x4 * VIEWING_DISTANCE / z4);
            y4 = (((float) WINDOW_HEIGHT / 2) + ASPECT_RATIO * y4 * VIEWING_DISTANCE / z4);

            int quadcolors[3] = {world_polys[curr_poly]->shade[2], world_polys[curr_poly]->shade[3], world_polys[curr_poly]->shade[0]};
            draw_triangle_3D_z((int) x3, (int) y3, (int) z3,(int) x4, (int) y4, (int) z4, (int) x1, (int) y1, (int) z1,
                             quadcolors, pixelmap, z_buffer, FLAT_SHADING);
        } // end if quad

    } // end for curr_poly
}

