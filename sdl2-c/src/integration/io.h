
#ifndef IO_H
#define IO_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../model/vector.h"
#include "../model/camera.h"
#include "SDL2/SDL_keyboard.h"

#define MOUSE_LEFT 0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT 2


/**
* IO structure.
* Idea is for an IO to correspond to 1 single vector (target) onto which the IO will act on. 
* 
* Vector of current mouse position.
* Array of current mouse button state.
* Keystate as an array to hold current keyboardstate.
* Function pointer for SDL_QUIT.
*/
typedef struct{
    Vector* mouse_positon;
    bool mousebutton_state[3];
    const Uint8* keystate;
    bool* quit;
    Camera* target_camera;
}IO;

/**
* Creates a new instance of IO.
* Idea is for an IO to correspond to camera (target) onto which the IO will act on. 
*
* Sets keystate as an array to hold current keyboardstate.
* Mousebutton state array is set to false.
* Initalizes vector for mouseposition.
*/
IO* io_create(bool* quit, Camera* target_camera);

/**
* Checks whether specific key is pressed given current keyboard state.
* @return true if key is down.
*/
bool io_is_key_down(IO* io, SDL_Scancode key);

/**
* Returns vector containing current mouse position.
*/
Vector* io_mouse_position(IO* io);

/**
* Switch Case table for all types of SDL Poll-Events.
*/
void io_event_update(IO* io);

/**
* Incorporates io_event_update together with outlined SDL_SCANCODE keys.
* Method basically handles controls.
*/
void io_handle_events(IO* io);

#endif