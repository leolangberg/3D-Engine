#include "../integration/display.h"
#include "../integration/io.h"
#include "../model/polygon.h"
#include "../model/camera.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define WINDOW_WIDTH 384
#define WINDOW_HEIGHT 216
#define ALL_PIXELS 82944
#define FPS 60
#define DELAY_TIME 1000.0f / FPS

#define BLUE   0xFFFF0000
#define GREEN 0xFF00FF00
#define RED  0xFF0000FF

#define OBJECT_LIST_MAX_SIZE 10

#define MAPWIDTH 24
#define MAPHEIGHT 24
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64









/**
* State structure (taken from 'Programming a first person shooter from scratch like it's 1995' by 'jdh').
* Holds a very simple SDL component structure for window, texture and renderer.
* Also contains pixelmap of entire screen (creating a first quadrant screen).
* Vector* camera_pos->z = camera distance.
*/
static struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
    bool quit;
    Camera* camera;
    IO* io; 

}state;


Object object;

void init() {


    state.camera = camera_init(vector_create(0,0,0));
    state.io = io_create(&state.quit, state.camera, &object);



    if(!PLG_Load_Object(&object, "/Users/leolangberg/Desktop/LinearAlgebra/sdl2-c/src/model/pyramid.plg", 1))
    {
        printf("\n Could not find file\n");
        return;
    }
    printf("\nobject created succesfully!\n");

    object_position(&object, 0, 0, 300);

    // view_point.x = 0;
    // view_point.y = 0;
    // view_point.z = 0;


    object_rotate_y(&object, (M_PI / 4));
    

    


}



/**
* Main method
* 
* Method firstly initializes SDL through SDL_Init.
* Then sets SDL window to given proportions, along with renderer and texture.
* Program Lifecycle is then run through a while loop that iterates as long as boolean quit is not true.
* Another while loop and switch case is used for event detection (using SDL_PollEvent)
* that finally sets lifecycle boolean to false if program is shut down.
* 
* SDL_UpdateTexture updates texture with current pixel state.
* SDL_RenderCopyEx then performs the actual rendering.
* memset() is then used to wipe the pixel array.
*/
int main( int arc, char* args[] ) {

    ASSERT(!SDL_Init(SDL_INIT_VIDEO),
           "SDL failed to initalize %s\n",
           SDL_GetError());
    
 
    state.window = SDL_CreateWindow("DEMO",
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   SCREENWIDTH,
                   SCREENHEIGHT,   
                   SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(
        state.window,
        "failed to create SDL window: %s\n",
        SDL_GetError());

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer,
           "failed to create SDL renderer: %s\n",
           SDL_GetError());

    state.texture = 
        SDL_CreateTexture(state.renderer,
            SDL_PIXELFORMAT_ABGR8888, 
            SDL_TEXTUREACCESS_STREAMING, 
            WINDOW_WIDTH, 
            WINDOW_HEIGHT);
    ASSERT(
        state.texture,
        "failed to create SDL texture %s\n",
        SDL_GetError());

    init();
    int index;


    /**
    * Engine Lifecycle loop.
    * 
    * Uses 64tick (60 fps) for improved framerate stability.
    * Performs io handling and object updates.
    * Updates and renders pixels.
    * Clears pixel array via memset.
    * Applies delay based on 64 tick.
    */
    uint64_t frameStart, frameTime;
    while(!state.quit)
    {
        frameStart = SDL_GetTicks64(); //SDL_GetTicks - Uint32.

        io_handle_events(state.io);
        camera_update(state.camera);

        // convert the local coordinates into world and camera coordinates for shading
        // and projection. note the viewer is at (0,0,0) with angles 0,0,0
        // so the transformation is imply to add the world position to each 
        // local vertex
        for(index = 0; index < object.num_vertices; index++)
        {
            object.vertices_camera[index].x = object.vertices_world[index].x = object.vertices_local[index].x + object.world_pos.x;
            object.vertices_camera[index].y = object.vertices_world[index].y = object.vertices_local[index].y + object.world_pos.y;
            object.vertices_camera[index].z = object.vertices_world[index].z = object.vertices_local[index].z + object.world_pos.z;
        }

        

        // shade and remove backfaces, ignore the backface part for now
        remove_backfaces_and_shade(&object);

        //draw the object
        draw_object_solid(&object, state.pixels);

    

        SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * 4);
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL); //makes window 1st quadrant.
        SDL_RenderPresent(state.renderer);

        memset(state.pixels, 0, sizeof(state.pixels));
        char title[250];
        snprintf(title, sizeof(title), "Pos: x=%.2f, y=%.2f, z=%.2f || Dir: x=%.2f, y=%.2f, z=%.2f || Target: x=%.2f, y=%.2f, z=%.2f || fYaw=%.2f", 
                    object.world_pos.x, 
                    object.world_pos.y, 
                    object.world_pos.z,
                    
                    state.camera->direction->x,
                    state.camera->direction->y,
                    state.camera->direction->z,
                    
                    state.camera->target->x,
                    state.camera->target->y,
                    state.camera->target->z,
                    
                    state.camera->fYaw);
        SDL_SetWindowTitle(state.window, title);

        frameTime = SDL_GetTicks64() - frameStart;
        if(frameTime < DELAY_TIME) {
            SDL_Delay((int) (DELAY_TIME - frameTime));
        }  
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    return 0;
}

