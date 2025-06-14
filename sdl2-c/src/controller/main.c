#include "../integration/plgreader.h"
#include "../integration/io.h"
#include "../model/object/polygon.h"
#include "../model/light/rgba.h"
#include "../model/camera.h"
#include "../model/global.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

// Static State structure.
// Holds a very simple SDL component structure for window, texture and renderer.
// Also contains pixelmap of entire screen (creating a first quadrant screen).
static struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    uint32_t pixels  [WINDOW_WIDTH * WINDOW_HEIGHT];
    uint32_t z_buffer[WINDOW_WIDTH * WINDOW_HEIGHT];
    bool quit;
    Camera* camera;
    IO* io; 
}state;

// Polygon i.e facet list containing all faces of all objects
// world_polys is pointer for real world_poly_storage.
int num_polys_frame = 0;
facet* world_polys[MAX_POLYS_PER_FRAME];
facet world_poly_storage[MAX_POLYS_PER_FRAME];
int z_buffer[WINDOW_WIDTH * WINDOW_HEIGHT]; 

RGBA palette[256];
//Vector source = {-0.913913,0.389759,-0.113369};
Vector source = {0,0,0};
float ambient_light = 6.0f;


// List of objects loaded in from PLG files.
Object test_objects[MAX_AMOUNT_OF_OBJECTS];
const int amount_of_objects = 1;

// Initialize camera, io and all external objects.
static inline void initialize_state() {
    Vector startpos = vector_create(0, 0, 0);
    state.camera    = camera_init(&startpos);
    state.io        = io_create(&state.quit, state.camera);
        
    // PLG_Load_Object(&test_objects[0], "src/assets/cube.plg", 1);
    OBJ_Load_Object(&test_objects[0], "src/assets/mountains.obj", 1);
    for(int index = 0; index < amount_of_objects; index++) {
        // PLG_Load_Object(&test_objects[index], "src/assets/cube.plg", 1);
        test_objects[index].world_pos.x=-200 + (index%4)*100;
        test_objects[index].world_pos.y=0;
        test_objects[index].world_pos.z=200 + 300*(index>>2);
        //test_objects[index].polys[0].two_sided = 1;
    }  
}

// A single instance or iteration of entire rendering process
// that is repeated continously throughout the program:
//    
// Each object is firstly culled to determine if it is even inside viewing window.
// Convert each object to world coordinates (sectors are already in world space).
// Shade and remove backfaces, ignore the backface part for now.
// Convert world coordinates into camera coordinates
// clip the object polygons against viewing volume.
// generate polygon list.
// clip possible near_z for each polygon.
static inline void lifecycle_process() {
    // Fill z_buffer with highest possible values.
    for(int i = 0; i < ALL_PIXELS; i++) {
        z_buffer[i] = INT_MAX;
    }

    // Handle all events regarding IO, update camera
    // and reset list of polygons.
    io_handle_events(state.io);
    camera_update(state.camera);
    reset_poly_list(&num_polys_frame);

    for(int index = 0; index < amount_of_objects; index++) {
        if(!object_culling(&test_objects[index], &state.camera->lookAt, OBJECT_CULL_XYZ_MODE)) 
        {
            mirror_two_sided_polygons(&test_objects[index]);
            printf("num polys: %d, verts: %d\n", test_objects[index].num_polys, test_objects[index].num_vertices);
            object_local_to_world_transformation(&test_objects[index]);
            remove_backfaces(&test_objects[index], &state.camera->position, CONSTANT_SHADING);
            light(&test_objects[index], palette, &source, ambient_light);
            object_view_transformation(&test_objects[index], &state.camera->lookAt);
            clip_object_3D(&test_objects[index], CLIP_XYZ_MODE);
            generate_poly_list(world_poly_storage, world_polys, &num_polys_frame, &test_objects[index]);
            clip_polygon(world_poly_storage, world_polys, &num_polys_frame);
        }
    }
    // draw polygon list with z-buffer.
    draw_poly_list_z(world_polys, &num_polys_frame, state.pixels, z_buffer);
}

// Confirm correct initalization of sdl variables via 
// the use of assertions.
static inline void initialize_sdl() {
    ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to initalize %s\n", SDL_GetError());
    state.window = SDL_CreateWindow("DEMO",
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   SDL_WINDOWPOS_CENTERED_DISPLAY(0), 
                   SCREENWIDTH,
                   SCREENHEIGHT,   
                   SDL_WINDOW_ALLOW_HIGHDPI);
    ASSERT(state.window, "failed to create SDL window: %s\n", SDL_GetError());
    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    ASSERT(state.renderer, "failed to create SDL renderer: %s\n", SDL_GetError());
    state.texture = SDL_CreateTexture(state.renderer,
                    SDL_PIXELFORMAT_ABGR8888, 
                    SDL_TEXTUREACCESS_STREAMING, 
                    WINDOW_WIDTH, 
                    WINDOW_HEIGHT);
    ASSERT( state.texture, "failed to create SDL texture %s\n", SDL_GetError());
}


// Update, render and present screen of pixels and then
// reset pixelmap. Also update tracking variables.
static inline void sdl_rendering_process() {
    SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * BYTE4);
    SDL_RenderCopyEx( state.renderer, state.texture, NULL, NULL, 0.0,  NULL, SDL_FLIP_VERTICAL); //makes window 1st quadrant.
    SDL_RenderPresent(state.renderer);
    memset(state.pixels, 0, sizeof(state.pixels));
    char title[100];
    snprintf(title, sizeof(title), "Pos: x=%.2f, y=%.2f, z=%.2f || Dir: x=%.2f, y=%.2f, z=%.2f || fYaw=%.2f || pitch=%.2f", 
                state.camera->position.x, 
                state.camera->position.y, 
                state.camera->position.z,
                state.camera->direction.x,
                state.camera->direction.y,
                state.camera->direction.z,
                state.camera->fYaw,
                state.camera->pitch);
    SDL_SetWindowTitle(state.window, title);
}

// Main method firstly initializes SDL through SDL_Init.
// Then sets SDL window to given proportions, along with renderer and texture.
// Program Lifecycle is then run through a while loop that iterates as long as boolean quit is not true.
// Another while loop and switch case is used for event detection (using SDL_PollEvent)
// that finally sets lifecycle boolean to false if program is shut down.
// SDL_UpdateTexture updates texture with current pixel state. SDL_RenderCopyEx then performs 
// the actual rendering. memset() is then used to wipe the pixel array.
int main( int arc, char* args[] ) {
    uint64_t frameStart, frameTime;
    initialize_sdl();
    initialize_state();
    Load_palette(palette, 256, "src/assets/grey256.pal");
    while(!state.quit)
    {
        frameStart = SDL_GetTicks64(); // SDL_GetTicks - Uint32.
        lifecycle_process();
        sdl_rendering_process();
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

