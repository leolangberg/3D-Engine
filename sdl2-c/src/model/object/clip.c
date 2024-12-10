#include "polygon.h"


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
        } // end else two sided
    } // end for curr_poly
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

void clip_polygon(facet *world_poly_storage, facet **world_polys, int *num_polys_frame) {
    // iterate through each polygon in render list
    int curr_poly, p_num_polys_frame = *num_polys_frame;   
    for(curr_poly = 0; curr_poly < p_num_polys_frame; curr_poly++)
    {
        int num_verts_outside = 0;          // counts how many points are outside.
        int vertex_ccodes[3] = {0,0,0};     // holds codes of which points that are outside.
        int v0, v1, v2;                     // indexes
        float t1, t2, xi, yi,
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
            { continue; }

            facet polygon;
            polygon.num_points  = 3;
            polygon.color       = world_polys[curr_poly]->color;
            polygon.shade[0]       = world_polys[curr_poly]->shade[0];
            polygon.shade[1]       = world_polys[curr_poly]->shade[1];
            polygon.shade[2]       = world_polys[curr_poly]->shade[2];
            polygon.shade[3]       = world_polys[curr_poly]->shade[3];
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
            polygon.shade[0] = world_polys[curr_poly]->shade[0];

            polygon.vertex_list[1].x = world_polys[curr_poly]->vertex_list[2].x;
            polygon.vertex_list[1].y = world_polys[curr_poly]->vertex_list[2].y;
            polygon.vertex_list[1].z = world_polys[curr_poly]->vertex_list[2].z;
            polygon.shade[1] = world_polys[curr_poly]->shade[2];

            polygon.vertex_list[2].x = world_polys[curr_poly]->vertex_list[3].x;
            polygon.vertex_list[2].y = world_polys[curr_poly]->vertex_list[3].y;
            polygon.vertex_list[2].z = world_polys[curr_poly]->vertex_list[3].z;
            polygon.shade[2] = world_polys[curr_poly]->shade[3];
            polygon.shade[3] = 0xFFFFFFFF;

            // add new polygon to pipeline
            world_poly_storage[p_num_polys_frame] = polygon;
            // assing pointer to it
            world_polys[p_num_polys_frame] = &world_poly_storage[p_num_polys_frame];
            // increment number of polys
            *num_polys_frame += 1;
            p_num_polys_frame++;
            
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
            if(num_verts_outside == 2) {
                // find out which of the points is the one still inside (0 == inside)
                // temp variables automatically point back to working variables.
                // Thus, you would not have to directly know which one is which, since
                // it knows that itself.  
                if(vertex_ccodes[0] == 0) {
                    v0 = 0; v1 = 1; v2 = 2;
                } else
                if(vertex_ccodes[1] == 0) {
                    v0 = 1; v1 = 2; v2 = 0;
                }else {
                    v0 = 2; v1 = 0; v2 = 1;
                };

                // step 2: clip each edge basically we are going to generate
                // the parametric line + = v0 + v01*t then solve for t when the z component 
                // is equal to CLIP_NEAR_Z, then plug that back into to solve for x,y of the 3D line.

                // clip ege v0->v1 and intersection occurs when z = CLIP_NEAR_Z
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);
                t1 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug back in and find x,y interseciton with the plane
                xi = world_polys[curr_poly]->vertex_list[v0].x + v.x * t1;
                yi = world_polys[curr_poly]->vertex_list[v0].y + v.y * t1;

                // now overwrite the vertex with new vertex
                world_polys[curr_poly]->vertex_list[v1].x = xi;
                world_polys[curr_poly]->vertex_list[v1].y = yi;
                world_polys[curr_poly]->vertex_list[v1].z = CLIP_NEAR_Z;

                // clip edge v0->v2 and intersection occurs when z = CLIP_NEAR_Z, so t:
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);
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
                temp_polygon.shade[0]      = world_polys[curr_poly]->shade[0];
                temp_polygon.shade[1]      = world_polys[curr_poly]->shade[1];
                temp_polygon.shade[2]      = world_polys[curr_poly]->shade[2];
                temp_polygon.shade[3]      = world_polys[curr_poly]->shade[3];
                temp_polygon.two_sided  = world_polys[curr_poly]->two_sided;
                temp_polygon.visible    = world_polys[curr_poly]->visible;
                temp_polygon.clipped    = world_polys[curr_poly]->clipped;
                temp_polygon.active     = world_polys[curr_poly]->active;

                temp_polygon.vertex_list[0] = world_polys[curr_poly]->vertex_list[0];
                temp_polygon.vertex_list[1] = world_polys[curr_poly]->vertex_list[1];
                temp_polygon.vertex_list[2] = world_polys[curr_poly]->vertex_list[2];

                // step 1: find vertex index for exterior vertex
                // z1 is outside of view frustum
                if(vertex_ccodes[0] == 1) {
                    v0 = 0; v1 = 1; v2 = 2;
                }
                else 
                // z2 is outside of view frustum
                if(vertex_ccodes[1] == 1) {
                    v0 = 1; v1 = 2; v2 = 0;
                }
                // z3 is outside of view frustum
                else {
                    v0 = 2; v1 = 0; v2 = 1;
                };
                // step 2: clip each edge basically we are going to generate the
                // parametric line p = v0 + v01 * t then solve for t when the z component is equal to
                // CLIP_NEAR_Z, then plug that back into to to solve for x,y of the 3D line.

                // clip edge v0->v1 and intersection occurs when z = CLIP_NEAR_Z, so t:
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v1]);
                t1 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug t back in and ifnd x,y intersection with the plane
                x01i = world_polys[curr_poly]->vertex_list[v0].x + v.x * t1;
                y01i = world_polys[curr_poly]->vertex_list[v0].y + v.y * t1;

                // clip edge v0->v2 and intersection occurs when z = CLIP_NEAR_Z, so t:
                v = *vector_sub(&world_polys[curr_poly]->vertex_list[v0], &world_polys[curr_poly]->vertex_list[v2]);
                t2 = ((CLIP_NEAR_Z - world_polys[curr_poly]->vertex_list[v0].z) / v.z);

                // now plug t back in and find x,y intersection with the plane
                x02i = world_polys[curr_poly]->vertex_list[v0].x + v.x * t2;
                y02i = world_polys[curr_poly]->vertex_list[v0].y + v.y * t2;

                // now we have both intersection points, we must overwrite the inplace
                // polygon's vertex 0 with the intersection point, this poly 1 of 2 from 
                // the split

                // now overwrite vertex with new vertex
                world_polys[curr_poly]->vertex_list[v0].x = x01i;
                world_polys[curr_poly]->vertex_list[v0].y = y01i;
                world_polys[curr_poly]->vertex_list[v0].z = CLIP_NEAR_Z;

                // now comes the hard part,
                // we have to carefully create a new polygon from the 2 intersection points and v2,
                // this polygon will be inserted at the end of the rendering list,
                // but for now, we are building it up in temp_polygon

                // so leave v2 alone, but overwrite v1 with v01, and overwrite v0 with v02
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

                // add new polygon to pipeline and assign pointer to it and icrement polys.
                world_poly_storage[p_num_polys_frame] = temp_polygon;
                world_polys[p_num_polys_frame] = &world_poly_storage[p_num_polys_frame];
                *num_polys_frame += 1;
                p_num_polys_frame++;
            } // end else if num_verts_outside == 1
            else { continue; }
        } // end if num_points == 3.
    } // end for loop
}