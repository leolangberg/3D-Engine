#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_error.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "../integration/display.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "../model/object.h"
#include "../integration/io.h"

#define WINDOW_WIDTH 384
#define WINDOW_HEIGHT 216
#define FPS 60
#define DELAY_TIME 1000.0f / FPS
#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }

#define OBJECT_LIST_MAX_SIZE 10


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
    Vector* camera_pos;
    IO* io;
    struct {
        Polygon3D** object_list;
        int list_length;
        void (*add)();
    }objects;

}state;

/**
* Function for adding objects to object list in 'state' struct.
* @param object object to be added.
*/
void list_add(void* object) {
    state.objects.object_list[state.objects.list_length] = object;
    state.objects.list_length++;
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
                   1280, 
                   720,
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

    state.camera_pos = vector_create(0, 0, 20);
    state.io = io_create(&state.quit, state.camera_pos);

    state.objects.object_list = malloc(sizeof(Polygon) * OBJECT_LIST_MAX_SIZE);
    state.objects.list_length = 0;
    state.objects.add = (list_add);
    

    Polygon3D* cube = object_create_cube(vector_create(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 50), 20);
    state.objects.add(cube);

    
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
    uint32_t color = 0x0000AAFF;
    while(!state.quit)
    {
        frameStart = SDL_GetTicks64(); //SDL_GetTicks - Uint32.

        io_handle_events(state.io);
        
        object_io(state.io, state.objects.object_list[0]);
        object_update_3d(state.objects.object_list[0]);
        //distance set to vector length of camera.
        object_draw_3d(state.pixels, state.objects.object_list[0], vector_length(state.camera_pos), color++);

        SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * 4);
        SDL_RenderCopyEx(
            state.renderer,
            state.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);

        memset(state.pixels, 0, sizeof(state.pixels));

        char title[100];
        snprintf(title, sizeof(title), "Object: x=%.2f, y=%.2f, z=%.2f || Camera: x=%.2f, y=%.2f, z=%.2f, length=%.2f", 
                    state.objects.object_list[0]->center->x, 
                    state.objects.object_list[0]->center->y, 
                    state.objects.object_list[0]->center->z,
                    state.camera_pos->x,
                    state.camera_pos->y,
                    state.camera_pos->z,
                    vector_length(state.camera_pos));
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