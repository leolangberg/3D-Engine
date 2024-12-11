#ifndef IO_H
#define IO_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../model/math/vector.h"
#include "../model/camera.h"
#include "../model/math/matrix.h"
#include "SDL2/SDL_keyboard.h"
#include "../model/object/polygon.h"

#define MOUSE_LEFT 0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT 2

// IO structure.
// IO to correspond to 1 single vector (target) onto which the IO will act on. 
// Vector of current mouse position. Array of current mouse button state.
// Keystate as an array to hold current keyboardstate. Function pointer for SDL_QUIT.
typedef struct{
    Vector       mouse_positon;
    bool         mousebutton_state[3];
    const Uint8* keystate;
    bool*        quit;
    Camera*      camera;
}IO;

// Creates a new instance of IO.
// Idea is for an IO to correspond to camera (target) onto which the IO will act on. 
// Sets keystate as an array to hold current keyboardstate. Mousebutton state array 
// is set to false. Initalizes vector for mouseposition. Sets quit callback function.
IO* io_create(bool* quit, Camera* camera);

// Retrieves latest keyboard state via SDL.
static inline void io_on_key_down(IO* io) { 
    io->keystate = SDL_GetKeyboardState(0); 
}

// Returns vector containing current mouse position.
static inline Vector* io_mouse_position(IO* io) {
    return &io->mouse_positon;
}

// Updates vector values of mouse position according to 
// current event status.
static inline void io_on_mouse_move(IO* io, SDL_Event* event) {
    io->mouse_positon.x = event->motion.x;
    io->mouse_positon.y = event->motion.y;
}
// Checks whether specific key is pressed given current keyboard state.
bool io_is_key_down(IO* io, SDL_Scancode key);
// Switch Case table for all types of SDL Poll-Events.
void io_event_update(IO* io);
// Incorporates io_event_update together with outlined SDL_SCANCODE keys.
// Method basically handles controls.
void io_handle_events(IO* io);

#endif