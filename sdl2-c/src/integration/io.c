#include "io.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_scancode.h"
#include <stdbool.h>
#include <stdlib.h>


/**
* Creates a new instance of IO.
*
* Sets keystate as an array to hold current keyboardstate.
* Mousebutton state array is set to false.
* Initalizes vector for mouseposition.
* Sets quit callback function.
* @param callback engine function for SDL_QUIT.
*/
IO* io_create(bool* quit, Vector* camera_pos) {
    IO* io = malloc(sizeof(IO));
    io->keystate = SDL_GetKeyboardState(0);
    io->mouse_positon = vector_create(0, 0, 0);
    for(int i = 0; i < 3; i++) {
        io->mousebutton_state[i] = false;
    }
    io->quit = quit;
    io->camera_pos = camera_pos;
    return io;
}

/**
* Returns vector containing current mouse position.
*/
Vector* io_mouse_position(IO* io) {
    return io->mouse_positon;
}

/**
* Updates vector values of mouse position according to current
* event status.
*/
void io_on_mouse_move(IO* io, SDL_Event* event) {
    io->mouse_positon->x = event->motion.x;
    io->mouse_positon->y = event->motion.y;
}

/**
* Updates mousebutton array with current event status.
*/
void io_on_mousebutton_down(IO*io, SDL_Event* event) {

    if(event->button.button == SDL_BUTTON_LEFT)   { io->mousebutton_state[MOUSE_LEFT] = true; }
    if(event->button.button == SDL_BUTTON_MIDDLE) { io->mousebutton_state[MOUSE_MIDDLE] = true;}
    if(event->button.button == SDL_BUTTON_RIGHT)  { io->mousebutton_state[MOUSE_RIGHT] = true; }
}

/**
* Retrieves latest keyboard state via SDL.
*/
void io_on_key_down(IO* io) { 
    io->keystate = SDL_GetKeyboardState(0); 
}

/**
* Checks whether specific key is pressed given current keyboard state.
* @return true if key is down.
*/
bool io_is_key_down(IO* io, SDL_Scancode key) {

    if(io->keystate != 0 && io->keystate != NULL) {
        if(io->keystate[key] == 1) {
            return true;
        }
        else {
            return false;
        }
    } 
    return false;   
}

/**
* Switch Case table for all types of SDL Poll-Events.
* case SDL_QUIT:            sets dereference of quit to true thus ending main lifecycle loop.
* case SDL_MOUSEMOTION:     updates mouse position vector with current mouse position.
* case SDL_MOUSEBUTTONDOWN: updates mousebutton_state array with current state of mouse buttons.
* case SDL_KEYDOWN:         updates keystate with current state of keys pressed.
*/
void io_event_update(IO* io) {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {   
        switch(event.type)
        {
            case SDL_QUIT:
               *io->quit = true;
            break;

            case SDL_MOUSEMOTION:
                io_on_mouse_move(io, &event);
            break;

            case SDL_MOUSEBUTTONDOWN:
                io_on_mousebutton_down(io, &event);
            break;

            case SDL_KEYDOWN:
                io_on_key_down(io);
            break;

            default:
            break;
        }
    }
}


/**
* Incorporates io_event_update together with outlined SDL_SCANCODE keys.
* Method basically handles controls.
*/
void io_handle_events(IO* io) {
    io_event_update(io);

    if(io_is_key_down(io, SDL_SCANCODE_Q)) {
        *io->quit = true;
    }

    if(io_is_key_down(io, SDL_SCANCODE_UP)) {
        io->camera_pos->z += 1;
    }

    if(io_is_key_down(io, SDL_SCANCODE_DOWN)) {
        io->camera_pos->z -= 1;
    }

    if(io_is_key_down(io, SDL_SCANCODE_LEFT)) {
        io->camera_pos->x -= 1;
    }

    if(io_is_key_down(io, SDL_SCANCODE_RIGHT)) {
        io->camera_pos->x += 1;
    }
}