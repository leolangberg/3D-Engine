#include "../integration/plgreader.h"
#include "../integration/display.h"
#include "../integration/io.h"
#include "../model/polygon.h"
#include "../model/camera.h"
#include "../model/global.h"
#include "../model/sector.h"
#include "../model/light/light.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_AMOUNT_OF_OBJECTS 24



/**
* State structure.
* Holds a very simple SDL component structure for window, texture and renderer.
* Also contains pixelmap of entire screen (creating a first quadrant screen).
*/
static struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
    bool quit;
    Camera* camera;
    IO* io; 

}state;

/*
* Externally declared variables:
*/
Object object;
Object test_objects[MAX_AMOUNT_OF_OBJECTS];
int num_polys_frame = 0;
int amount_of_objects = 16;



void init() {


    state.camera = camera_init(vector_create(0,0,0));
    state.io = io_create(&state.quit, state.camera);
    
    
    for(int index = 0; index < amount_of_objects; index++)
    {
        PLG_Load_Object(&test_objects[index], "src/assets/pyramid.plg", 1);
    }


    
    for(int index=0; index<amount_of_objects-1; index++)
    {
        test_objects[index].world_pos.x=-200 + (index%4)*100;
        test_objects[index].world_pos.y=0;
        test_objects[index].world_pos.z=200 + 300*(index>>2);
        test_objects[index].polys[0].two_sided = 1;
    }

    
    

    RGB white, gray, black, red, green, blue;
    white.rgba = _RGB32BIT(0, 255, 255, 255);
    gray.rgba  = _RGB32BIT(0, 100, 100, 100);
    black.rgba = _RGB32BIT(0, 0, 0, 0);
    red.rgba   = _RGB32BIT(0, 255, 0, 0);
    green.rgba = _RGB32BIT(0, 0, 255, 0);
    blue.rgba  = _RGB32BIT(0, 0, 0, 255);

    Reset_Lights();

    // ambient light
    light_construct(0, 1, LIGHTV1_ATTR_AMBIENT, 
                                       white, black, black,
                                       NULL, NULL,
                                       0, 0, 0,
                                       0, 0, 0);
    
    // directional light
    Vector dlight_dir = {-1, -1, -1};
    light_construct(1, 1, LIGHTV1_ATTR_INFINITE,
                                        black, white, black,
                                        NULL, &dlight_dir,
                                        0, 0, 0,
                                        0, 0, 0);

    // point light
    Vector plight_pos = {0, 200, 400};
    light_construct(2, 1, LIGHTV1_ATTR_POINT, 
                                       black, white, black,
                                       &plight_pos, NULL,
                                        0, 0.001, 0,
                                        0, 0, 100);
    
    // spot light
    Vector slight_pos = {0, 200, 400};
    Vector slight_dir = {-1, -1, -1};
    light_construct(3, 1, LIGHTV1_ATTR_SPOTLIGHT2,
                                    black, white, black,
                                    &slight_pos, &slight_dir,
                                    0, 0.001, 0,
                                    0, 0, 100);

    // Load palette from disk and build lookup table
    if(!Load_palette_from_file("src/assets/standard.pal", standard_pal)) {
        printf("palette could not be loaded.\n");
    }
    if(!Build_RGB_Lookup_Table(1, standard_pal, rgblookup)) {
        printf("RGB Lookup Table could not be created.\n");
    }
    if(!Build_RGB_Intensity_Lookup_Table(standard_pal, rgbilookup, 1)) {
        printf("RGB Lookup Table could not be created.\n");
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

        fill_z_buffer(16000);
        io_handle_events(state.io);
        camera_update(state.camera);

        generate_poly_list(NULL, RESET_POLY_LIST);
        
        for(int index = 0; index < amount_of_objects; index++)
        {
            if(!object_culling(&test_objects[index], state.camera->lookAt, OBJECT_CULL_XYZ_MODE))
            {
                // convert to world coordinates.
                // SECTORS ARE ALREADY IN WORLD SPACE
                object_local_to_world_transformation(&test_objects[index]);
                // shade and remove backfaces, ignore the backface part for now
                remove_backfaces_and_shade(&test_objects[index], state.camera->position, CONSTANT_SHADING);
                // Apply lighting to objects.
                Light_Object_world8(&test_objects[index], state.camera, lights, num_lights, FLAT_SHADING, rgblookup);
                // convert world coordinates to camera coordinates.
                object_view_transformation(&test_objects[index], state.camera->lookAt);
                //clip the object polygons against viewing volume
                clip_object_3D(&test_objects[index], CLIP_XYZ_MODE);
                // generate poly list
                generate_poly_list(&test_objects[index], 1);
                // clip near z possible polygons.
                clip_polygon();
            }
        }

        // draw polygon list with z-buffer.
        draw_poly_list_z(state.pixels);

        
        
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
        char title[250];
        snprintf(title, sizeof(title), "Pos: x=%.2f, y=%.2f, z=%.2f || Dir: x=%.2f, y=%.2f, z=%.2f || fYaw=%.2f || pitch=%.2f", 
                    state.camera->position->x, 
                    state.camera->position->y, 
                    state.camera->position->z,
                    
                    state.camera->direction->x,
                    state.camera->direction->y,
                    state.camera->direction->z,
                    
                    state.camera->fYaw,
                    state.camera->pitch);
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

