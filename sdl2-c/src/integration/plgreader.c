#include "plgreader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
* Reads line of PLG file and converts file text into string. 
*/
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


/**
* Beware of the strtok as it carves the buffer up and leaves only single words for scanff.
*/
int PLG_Load_Sector(Sector* sector, char *filename) {
    
    FILE *fp;               // disk file
    char buffer[80];        // holds input string

    int wall_index = 0;         // current row of wall reading


    // open the disk file
    if((fp=fopen(filename, "r")) == NULL) {
        printf("Could not open file %s\n", filename);
        return 0;
    }

    // read line 
    if(!PLG_Get_Line(buffer, 80, fp)) {
        printf("Error with PLG file %s (stop 1)", filename);
        fclose(fp);
        return 0;
    }

    int index;
        
        // read line 
    sscanf(buffer, "%d %d %d %d %d", &sector->id, &index, &sector->num_walls, &sector->floor, &sector->ceiling);
  
    for(int i = 0; i < sector->num_walls; i++) 
    {
        // read line 
        if(!PLG_Get_Line(buffer, 80, fp)) {
            printf("Error with PLG file %s (stop 1)", filename);
            fclose(fp);
            return 0;
        }
        
        int x1, z1, 
            x2, z2, 
            link;

        // read line
        sscanf(buffer, "%d %d %d %d %d", &x1, &z1, &x2, &z2, &link);

        // declare object 
        Object object;
        object.id = sector->id;
        object.num_vertices = 4;
        object.num_polys = 1;
        object.state = 1;

        object.vertices_world[0].x = x1;
        object.vertices_world[0].y = sector->floor;
        object.vertices_world[0].z = z1;

        object.vertices_world[1].x = x2;
        object.vertices_world[1].y = sector->floor;
        object.vertices_world[1].z = z2;

        object.vertices_world[2].x = x2;
        object.vertices_world[2].y = sector->ceiling;
        object.vertices_world[2].z = z2;

        object.vertices_world[3].x = x1;
        object.vertices_world[3].y = sector->ceiling;
        object.vertices_world[3].z = z1;

        // declare polygon 
        Polygon polygon;
        Vector u,v;

        polygon.num_points = 4;
        polygon.vertex_list[0] = 0;
        polygon.vertex_list[1] = 1;
        polygon.vertex_list[2] = 2;
        polygon.vertex_list[3] = 3;

        polygon.color     = 0xFF444444;
        polygon.shade[0]     = 0;
        polygon.shade[1]     = 0;
        polygon.shade[2]     = 0;
        polygon.shade[3]     = 0;
        polygon.two_sided = 1;
        polygon.visible   = 1;
        polygon.clipped   = 0;
        polygon.active    = 1;

        u = *vector_sub(&object.vertices_world[0], &object.vertices_world[1]);
        v = *vector_sub(&object.vertices_world[0], &object.vertices_world[2]);
        polygon.normal = *vector_cross_product(&u, &v);
        
        // place polygon in object 
        object.polys[0] = polygon;
        object.world_pos.x = 0;
        object.world_pos.y = 0;
        object.world_pos.z = 0;
        object.radius = 1; //compute_object_radius(&object);

        // place object in sector object list
        sector->wall_list[wall_index] = object;
        wall_index++;

    }
    return 1;

}

/**
* Loads PLG files by reading from the text file and declaring variables accordingly.
* Also has the option to scale the object as it is being constructed.
*/
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