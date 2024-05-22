
#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../model/object.h"
#include "../integration/io.h"

#define INITIAL_OBJECT_LIST_MAX 10

/**
* Engine structure.
* 
* Window (width and height) and Renderer.
* Boolean that represents the program running.
* List containing all objects in program.
* IO: input and output from user.
*/
typedef struct{

    int window_width;
    int window_height;

    SDL_Window *g_pWindow;
    SDL_Renderer *g_pRenderer;
    bool g_bIsRunning;

    int object_list_length;
   // Object* object_list;    

    IO* io;

}Engine;

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
*/
Engine* engine_init(const char* title, int xpos, int ypos, int width, int height, int flags);

/**
* Iteraties through all objects and their update methods.
*/
void engine_update(Engine* engine);

/**
* Iterates through all objects and their render methods then 
* presents the result.
*/
void engine_render(Engine* engine);

/**
* Cleans engine by calling all objects to clean.
* Window and Renderer are then destroyed and engine is freed.
*/
void engine_clean(Engine* engine);

/**
* Checks Poll-Events for IO handling.
*/
void engine_handle_events(Engine* engine);



#endif