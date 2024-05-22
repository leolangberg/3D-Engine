#include <stdlib.h>
#include "engine.h"

/**
* Iteraties through all objects and their update methods.
*/
void engine_update(Engine* engine) {
    int size = engine->object_list_length;
    for(int i = 0; i < size; i++)
    {
        //engine->object_list[i].update();
    }
    
}

/**
* Iterates through all objects and their render methods then 
* presents the result.
*
* RenderClear works by setting the screen color to the previously set color
* because of how SDL2_gfx interacts to ensure no errors 
* setRenderDrawColor is used after rendering.
*/
void engine_render(Engine* engine) {

    SDL_RenderClear(engine->g_pRenderer);
    int size = engine->object_list_length;
    for(int i = 0; i < size; i++)
    {
        //engine->object_list[i].render();
    }
    SDL_RenderPresent(engine->g_pRenderer);
    SDL_SetRenderDrawColor(engine->g_pRenderer, 0, 0, 0, 255);
}

/**
* Cleans engine by calling all objects to clean.
* Window and Renderer are then destroyed and engine is freed.
*/
void engine_clean(Engine* engine) {

    int size = engine->object_list_length;
    for(int i = 0; i < size; i++)
    {
        //engine->object_list[i].clean();
    }
    SDL_DestroyWindow(engine->g_pWindow);
    SDL_DestroyRenderer(engine->g_pRenderer);
    free(engine);
    SDL_Quit();
}

/**
* Initializes the SDL_Window and SDL_Renderer.
*
* @param title gives title of window.
* @param xpos x-position.
* @param ypos y-position.
* @param width width of window.
* @param height height of window.
* @param flags type of SDL_Window (SHOWN, etc.)
* @return boolean to determine if method was successful.
*/
bool window_init(Engine* engine, const char* title, int xpos, int ypos, int width, int height, int flags) {
    if(SDL_Init(SDL_INIT_EVERYTHING) >= 0) 
    {
        engine->g_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if( engine->g_pWindow != 0) {
            engine->g_pRenderer = SDL_CreateRenderer(engine->g_pWindow, -1, 0);
        }
    }
    else {
        return false;
    }
    return true;
}


/**
* Initializes Engine.
*
* Creates new Engine together with SDL_Window and SDL_Renderer
* then also creates object list for all objects in program.
* @param title gives title of window.
* @param xpos x-position.
* @param ypos y-position.
* @param width width of window.
* @param height height of window.
* @param flags type of SDL_Window (SHOWN, etc.)
* @return initialized Engine.
*
* engine->object_list_length = 0 so that it does not allow for 
* any BAD_ACCESS memory leaks. 
*/
Engine* engine_init(const char* title, int xpos, int ypos, int width, int height, int flags) {
    Engine* engine = malloc(sizeof(Engine));

    if(window_init(engine, title, xpos, ypos, width, height, flags))
    {
        engine->g_bIsRunning = true;
    } else {
        return 0; //sdl could not initialize
    }

    engine->window_width = width;
    engine->window_height = height;
    
    engine->object_list_length = 0;
   // engine->object_list = malloc(INITIAL_OBJECT_LIST_MAX * sizeof(Object));

    engine->io = io_create((void*) engine_clean, engine);
    return engine;
}