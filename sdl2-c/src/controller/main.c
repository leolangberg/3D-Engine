#include "../integration/plgreader.h"
#include "../integration/display.h"
#include "../integration/io.h"
#include "../model/polygon.h"
#include "../model/camera.h"
#include "../model/global.h"
#include "../model/sector.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_AMOUNT_OF_OBJECTS 24



/**
* State structure.
* Holds a very simple SDL component structure for window, texture and renderer.
* Also contains pixelmap of entire screen (creating a first quadrant screen).
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

/*
* Externally declared variables:
*/
Object object;
Object test_objects[MAX_AMOUNT_OF_OBJECTS];
Vector light_source = {-0.913913, 0.389759, -0.113369};
float ambient_light = 6;
int num_polys_frame = 0;

int amount_of_objects = 16;


void init() {


    state.camera = camera_init(vector_create(0,0,0));
    state.io = io_create(&state.quit, state.camera);
      
    
    
    for(int index = 0; index < amount_of_objects; index++)
    {
        PLG_Load_Object(&test_objects[index], "src/assets/cube.plg", 1);
    }
 

    for(int index=0; index<amount_of_objects; index++)
    {
        test_objects[index].world_pos.x=-200 + (index%4)*100;
        test_objects[index].world_pos.y=0;
        test_objects[index].world_pos.z=200 + 300*(index>>2);
        //test_objects[index].polys[0].two_sided = 1;
    }

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

        fill_z_buffer(16000);
        io_handle_events(state.io);
        camera_update(state.camera);

        generate_poly_list(NULL, RESET_POLY_LIST);
        
        for(int index = 0; index < amount_of_objects; index++)
        {
            if(!object_culling(&test_objects[index], state.camera->lookAt, OBJECT_CULL_XYZ_MODE))
            {
                // convert to world coordinates.
                object_local_to_world_transformation(&test_objects[index]);
                // shade and remove backfaces, ignore the backface part for now
                remove_backfaces_and_shade(&test_objects[index], state.camera->position, CONSTANT_SHADING);
                // convert world coordinates to camera coordinates.
                object_view_transformation(&test_objects[index], state.camera->lookAt);
                //clip the object polygons against viewing volume
                clip_object_3D(&test_objects[index], CLIP_XYZ_MODE);
                // generate poly list
                generate_poly_list(&test_objects[index], 1);
                // clip near z possible polygons.
                clip_polygon();
            }
        }

        draw_poly_list_z(state.pixels);

        
        
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
        snprintf(title, sizeof(title), "Pos: x=%.2f, y=%.2f, z=%.2f || Dir: x=%.2f, y=%.2f, z=%.2f || fYaw=%.2f", 
                    state.camera->position->x, 
                    state.camera->position->y, 
                    state.camera->position->z,
                    
                    state.camera->direction->x,
                    state.camera->direction->y,
                    state.camera->direction->z,
                    
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

