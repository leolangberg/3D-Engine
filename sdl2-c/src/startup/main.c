#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "../integration/display.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "../model/object.h"
#include <time.h>

#define WINDOW_WIDTH 384
#define WINDOW_HEIGHT 216
#define FPS 60
#define DELAY_TIME 1000.0f / FPS

/**
* State structure (taken from 'Programming a first person shooter from scratch like it's 1995' by 'jdh').
* Holds a very simple SDL component structure for window, texture and renderer.
* Also contains pixelmap of entire screen (creating a first quadrant screen).
*/
static struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
    bool quit;

    Vector* pos;

}state;

/**
* Renders a line on the screen.
*/
static void render_line(int x, int y0, int y1, uint32_t color) {
    for(int y = y0; y <= y1; y++) {
        state.pixels[(y * WINDOW_WIDTH) + x] = color;
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
*/
int main( int arc, char* args[] ) {

    SDL_Init(SDL_INIT_VIDEO);
 
    state.window = SDL_CreateWindow("DEMO",
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   1280, 
                   720,
                   SDL_WINDOW_ALLOW_HIGHDPI);

    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);

    state.texture = 
        SDL_CreateTexture(state.renderer,
            SDL_PIXELFORMAT_ABGR8888, 
            SDL_TEXTUREACCESS_STREAMING, 
            WINDOW_WIDTH, 
            WINDOW_HEIGHT);

    state.pos = vector_create(0, 0, 0);

    Polygon* triangle = object_create_triangle();
    uint32_t color = 0xFACDA000;

    while(!state.quit)
    {
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_QUIT:
                    state.quit = true;
                break;
            }
        }   
    }
    
    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);

    return 0;
}