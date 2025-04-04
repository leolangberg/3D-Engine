#include "plgreader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Reads line of PLG file and converts file text into string. 
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

// count how many lines there are in file.
// helps with determining size for vector array.
int OBJ_get_amount_polys(char *filename) {
    
    int num_polys = 0;
    char buffer[80];          // holds input string
    FILE *fp;

    // open the disk file
    if((fp=fopen(filename, "r")) == NULL) {
        printf("could not open file %s\n", filename);
        return 0;
    }
    while(1) {
        if(PLG_Get_Line(buffer, 80, fp) == NULL) {
            break;
        }
        if(buffer[0] == 'f') {
            num_polys++;
        }
    }

    fclose(fp);
    return num_polys;

}

// count how many lines there are in file.
// helps with determining size for vector array.
int OBJ_get_amount_vertices(char *filename) {
    
    int num_vertices = 0;
    char buffer[80];          // holds input string
    FILE *fp;

    // open the disk file
    if((fp=fopen(filename, "r")) == NULL) {
        printf("could not open file %s\n", filename);
        return 0;
    }
    while(1) {
        if(PLG_Get_Line(buffer, 80, fp) == NULL) {
            break;
        }
        if(buffer[0] == 'v') {
            num_vertices++;
        }
    }

    fclose(fp);
    return num_vertices;

}

int OBJ_Load_Object(Object* object, char *filename, float scale) {
    // this function loads an object off disk and allows it to be scaled.

    FILE *fp; // disk file
    static int id_number = 0; // used to set object id's
    char buffer[80],          // holds input string
         type;
    float x,y,z;              // a single vertex
    int tl,
        tr,
        br;
    int vertex_0,
        vertex_1,
        vertex_2;

    Vector u,v,normal;              // working vectors

    const int num_vertices = OBJ_get_amount_vertices(filename);
    const int num_polys    = OBJ_get_amount_polys(filename);
    printf("verts: %d, polys: %d\n", num_vertices, num_polys);

    int ver_index = 0;
    int poly_index = 0;

    object->num_vertices = num_vertices;
    object->num_polys = num_polys;
    object->state = 1;

    object->world_pos.x = 0;
    object->world_pos.y = 0;
    object->world_pos.z = 0;

    object->id = id_number++;


    // open the disk file
    if((fp=fopen(filename, "r")) == NULL) {
        printf("could not open file %s\n", filename);
        return 0;
    }

    // Find and add all vertice into the vertice array.
    // Once "s off" is reached 
    while(1) {
        if(PLG_Get_Line(buffer, 80, fp) == NULL) {
            break;
        }
        if(sscanf(buffer, "%c %f %f %f", &type, &x, &y, &z) != 4) {
            break;
        }
        // Vertice, add it to array.
        if(type == 'v') {
            object->vertices_local[ver_index].x = x * scale;
            object->vertices_local[ver_index].y = y * scale;
            object->vertices_local[ver_index].z = z * scale;
            ver_index++;
        }
    }

    // Iterate through all polys and add them to the object.
    while(1) {
        if(PLG_Get_Line(buffer, 80, fp) == NULL) {
            break;
        }
        if(sscanf(buffer, "%c %d %d %d", &type, &tl, &tr, &br) != 4) {
            break;
        }
        if(type == 'f') {

            object->polys[poly_index].num_points = 3;
            object->polys[poly_index].color      = 0x0000FF00;
            object->polys[poly_index].two_sided  = 0;
            object->polys[poly_index].visible    = 1;
            object->polys[poly_index].clipped    = 0;
            object->polys[poly_index].active     = 1;

            // Top left corner = vertex_list[0]
            // Also -1 for each value because file is 1' index and objects are 0'.
            object->polys[poly_index].vertex_list[0] = tl-1;
            object->polys[poly_index].vertex_list[1] = tr-1;
            object->polys[poly_index].vertex_list[2] = br-1;


            vertex_0 = object->polys[poly_index].vertex_list[0];
            vertex_1 = object->polys[poly_index].vertex_list[1];
            vertex_2 = object->polys[poly_index].vertex_list[2];
    
            // the vector u = v0->v1
            u = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_1]);
            // the vector v = v0->v2
            v = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_2]);
    
            normal = vector_cross_product(&v, &u);
            object->polys[poly_index].normal = normal;

            poly_index++;

        }

    }

    fclose(fp);   
    object->radius = compute_object_radius(object);
    return 1;
    
}

// Loads PLG files by reading from the text file and declaring variables accordingly.
// Also has the option to scale the object as it is being constructed.
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
                 logical_color,     // the final color of polygon
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

    //printf("total_vertices: %d, total_polys: %d\n", total_vertices, total_polys);
    
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

        //printf("x: %.2f, y: %.2f, z: %.2f\n", x, y , z);

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
            sscanf(&token[2], "%x", &logical_color);
            // end if hex color specifier
        }
        else {
            logical_color = atoi(token);
            // end if decimal
        }

        
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
        object->polys[index].two_sided  = 0;
        object->polys[index].visible    = 1;
        object->polys[index].clipped    = 0;
        object->polys[index].active     = 1;

        //printf("num_points: %d\n", num_vertices);

        // now read in polygon vertice list
        for(index_2 = 0; index_2 < num_vertices; index_2++)
        {
            // read in next vertex number
            if(!(token = strtok(NULL, " ")))
            {
                printf("Error with PLG file %s (stop 7)", filename);
                fclose(fp);
                return 0;
            }

            vertex_num = atoi(token);
            //printf("vertex_num: %d\n", vertex_num);

            // insert vertex number into polygon
            object->polys[index].vertex_list[index_2] = vertex_num;
        }

        // compute length of the two co-planar edges of the polygon, since they
        // will be used in the computation of the dot-product later

        vertex_0 = object->polys[index].vertex_list[0];
        vertex_1 = object->polys[index].vertex_list[1];
        vertex_2 = object->polys[index].vertex_list[2];

        // the vector u = v0->v1
        u = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_1]);
        // the vector v = v0->v2
        v = vector_sub(&object->vertices_local[vertex_0], &object->vertices_local[vertex_2]);

        normal = vector_cross_product(&v, &u);
        object->polys[index].normal = normal;
    }

    // close file
    fclose(fp);

    // compute object radius
    object->radius = compute_object_radius(object);

    return 1;
}
