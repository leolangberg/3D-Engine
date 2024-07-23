#include "../integration/display.h"
#include "../integration/io.h"
#include "../model/object.h"
#include "../model/camera.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
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

#define OBJECT_LIST_MAX_SIZE 10

#define MAPWIDTH 24
#define MAPHEIGHT 24
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64




int worldMap[MAPWIDTH][MAPHEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//no point in having a buffer as pixels are already drawn one by one. 
int texture[8][TEXTURE_WIDTH * (TEXTURE_HEIGHT - 1) + (TEXTURE_WIDTH - 1)];



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
    struct {
        Object** object_list;
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

void init() {

    state.camera = malloc(sizeof(Camera));
    state.camera->position = vector_create(12, 12, 0);
    state.camera->direction = vector_create(-1, 0, 0);
    state.camera->camera_plane = vector_create(0, 1, 0); //perpendicular to direction. (90')
    state.camera->speed = 0.1;
    
    state.io = io_create(&state.quit, state.camera, worldMap);

    state.objects.object_list = malloc(sizeof(Polygon) * OBJECT_LIST_MAX_SIZE);
    state.objects.list_length = 0;
    state.objects.add = (list_add);


    for(int x = 0; x < TEXTURE_WIDTH; x++)
    {
        for(int y = 0; y < TEXTURE_HEIGHT; y++)
        {
            int xorcolor = (x * 256 / TEXTURE_WIDTH) ^ (y * 256 / TEXTURE_HEIGHT);
            //int xcolor = x * 256 / texWidth;
            int ycolor = y * 256 / TEXTURE_HEIGHT;
            int xycolor = y * 128 / TEXTURE_HEIGHT + x * 128 / TEXTURE_WIDTH;
            texture[0][TEXTURE_WIDTH * y + x] = 65536 * 254 * (x != y && x != TEXTURE_WIDTH - y); //flat red texture with black cross
            texture[1][TEXTURE_WIDTH * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
            texture[2][TEXTURE_WIDTH * y + x] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
            texture[3][TEXTURE_WIDTH * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
            texture[4][TEXTURE_WIDTH * y + x] = 256 * xorcolor; //xor green
            texture[5][TEXTURE_WIDTH * y + x] = 65536 * 192 * (x % 16 && y % 16); //red bricks
            texture[6][TEXTURE_WIDTH * y + x] = 65536 * ycolor; //red gradient
            texture[7][TEXTURE_WIDTH * y + x] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
        }
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
        io_handle_events(state.io);

        raycasting_algorithm(state.camera, worldMap, state.pixels, texture);

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
        char title[100];
        snprintf(title, sizeof(title), "Pos: x=%.2f, y=%.2f, z=%.2f || Dir: x=%.2f, y=%.2f, z=%.2f || Plane: x=%.2f, y=%.2f, z=%.2f", 
                    state.camera->position->x, 
                    state.camera->position->y, 
                    state.camera->position->z,
                    
                    state.camera->direction->x,
                    state.camera->direction->y,
                    state.camera->direction->z,
                    
                    state.camera->camera_plane->x,
                    state.camera->camera_plane->y,
                    state.camera->camera_plane->z);
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

