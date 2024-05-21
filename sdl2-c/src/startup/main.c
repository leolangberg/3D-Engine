#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "../controller/engine.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60
#define DELAY_TIME 1000.0f / FPS


/**
* Main method
* 
* Runs entire process through a while loop continously calling the engine
* to perform an update and render each iteration.
*/
int main( int arc, char* args[] ) {

    printf("START PROGRAM\n");
    Engine* engine = engine_init(" Chapter 1: Setting up SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Event windowEvent;
    while( engine->g_bIsRunning )
    {
        engine_update(engine);
        engine_render(engine);

        if( SDL_PollEvent( &windowEvent) ) {
            if( SDL_QUIT == windowEvent.type )
            { break; }
        }
    }

    engine_clean(engine);
    return 0;

}