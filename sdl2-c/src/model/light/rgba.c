#include "rgba.h"
#include <stdlib.h>
#include <stdio.h>

// Loads palette from file to array.
void Load_palette(RGBA *palette, const int pal_length, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    for (int i = 0; i < pal_length; i++) {
        fscanf(file, "%d %d %d %d", (int*) &palette[i].a, (int*) &palette[i].b, (int*) &palette[i].g, (int*) &palette[i].r);    
    }
    fclose(file);
}

// translate color from palette into straight 32 bit color integer.
int palette_get_color(RGBA *palette, const int index) {
    int color = (palette[index].r) | (palette[index].g << 8) | (palette[index].b << 16) | (palette[index].a << 24);
    return color;

}

void light(Object *object, RGBA *palette, Vector *light_source, float ambient_light) {
    // we need to compute the normal of this polygon face, and recall
    // that the vertices should be in counter-clockwise order:
    // u = p0->p1, v = p0->p2, n = u x v  sight = p0->source;
    Vector u, v, normal, norm, sight, to_source;
    int vertex_0, vertex_1, vertex_2;
    const int point_light = 16;
    float dp, mg, intensity;


    for(int curr_poly = 0; curr_poly < object->num_polys; curr_poly++) {
        
        // Intensity initalially set to ambient level.
        intensity = ambient_light;
        
        // Object is not visible.
        if(object->polys[curr_poly].clipped || (!object->polys[curr_poly].active) ||
         (!object->polys[curr_poly].visible)) { 
            continue; 
        }

        // extract vertex indices into master list, remember the polygons are 
        // NOT self contained, but based on the vertex list stored in the object itself.
        vertex_0 = object->polys[curr_poly].vertex_list[0];
        vertex_1 = object->polys[curr_poly].vertex_list[1];
        vertex_2 = object->polys[curr_poly].vertex_list[2];


        u = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
        v = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);
        normal = vector_cross_product(&v, &u); // originally u x v but n grows in wrong direction then?
        norm = vector_normalize(&normal);
        to_source = vector_sub(&object->vertices_world[vertex_0], light_source);
        dp = vector_dot_product(&norm, &to_source);

        // Directional light hits object.
        if(dp > 0) {
            // now cos() = (w*v) / ||w|| ||v||
            mg = vector_length(&norm) * vector_length(&to_source);
            intensity += (point_light * (dp / mg));
        } 
        
        if(intensity > 16) { intensity = 16; } 
        object->polys[curr_poly].shade[0] = palette_get_color(palette, ((16 * (int) intensity) - 1));


        // ###################################################################
        
        // Enligt chat: for eaech vertex, you average the normals of all the polygons
        //              that share that vertex. (Run through vertex list?)
        // For gouraud shading then we need to know intensity for each vertex.
        vertex_0 = object->polys[curr_poly].vertex_list[1];
        vertex_1 = object->polys[curr_poly].vertex_list[2];
        vertex_2 = object->polys[curr_poly].vertex_list[0];


        u = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
        v = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);
        normal = vector_cross_product(&v, &u); // originally u x v but n grows in wrong direction then?
        norm = vector_normalize(&normal);
        to_source = vector_sub(&object->vertices_world[vertex_0], light_source);
        dp = vector_dot_product(&norm, &to_source);

        // Directional light hits object.
        if(dp > 0) {
            // now cos() = (w*v) / ||w|| ||v||
            mg = vector_length(&norm) * vector_length(&to_source);
            intensity += (point_light * (dp / mg));
        } 

        if(intensity > 16) { intensity = 16; } 
        object->polys[curr_poly].shade[1] = palette_get_color(palette, ((16 * (int) intensity) - 1));


        // For gouraud shading then we need to know intensity for each vertex.
        vertex_0 = object->polys[curr_poly].vertex_list[2];
        vertex_1 = object->polys[curr_poly].vertex_list[0];
        vertex_2 = object->polys[curr_poly].vertex_list[1];


        u = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
        v = vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);
        normal = vector_cross_product(&v, &u); // originally u x v but n grows in wrong direction then?
        norm = vector_normalize(&normal);
        to_source = vector_sub(&object->vertices_world[vertex_0], light_source);
        dp = vector_dot_product(&norm, &to_source);

        // Directional light hits object.
        if(dp > 0) {
            // now cos() = (w*v) / ||w|| ||v||
            mg = vector_length(&norm) * vector_length(&to_source);
            intensity += (point_light * (dp / mg));
        } 

        if(intensity > 16) { intensity = 16; } 
        object->polys[curr_poly].shade[2] = palette_get_color(palette, ((16 * (int) intensity) - 1));
    }
}




