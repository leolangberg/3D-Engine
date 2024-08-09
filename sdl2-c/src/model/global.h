
#ifndef GLOBAL_H
#define GLOBAL_H


#define ASSERT(_e, ...) if (!(_e)) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define FPS 60
#define DELAY_TIME 1000.0f / FPS

#define SCREENWIDTH 1280            // real size of window (only relevant for SDL)   
#define SCREENHEIGHT 720

#define WINDOW_WIDTH  384                             // pixel width of window (Relevant for code)
#define WINDOW_HEIGHT 216                             // pixel height of window
#define ALL_PIXELS ((WINDOW_WIDTH) * (WINDOW_HEIGHT)) // total num of pixels in pixel array

#define VIEWING_DISTANCE     250    // View distance (FOV)
#define ASPECT_RATIO         1      // ratio between width and height
#define INVERSE_ASPECT_RATIO 1

#define CLIP_FAR_Z 1000.0f                  // max z distance of objects in view
#define CLIP_NEAR_Z 1.0f                    // min z distance of objects in view
#define poly_clip_max_x WINDOW_WIDTH - 1    // max x
#define poly_clip_max_y WINDOW_HEIGHT - 1   // max y
#define poly_clip_min_x 0                   // min x
#define poly_clip_min_y 0                   // min y

#define OBJECT_CULL_Z_MODE   0              // only removes or "culls" objects based on z layer.
#define OBJECT_CULL_XYZ_MODE 1              // also removes based on x and y along with z.
#define CLIP_Z_MODE 0                       // clips object based on z layer.  
#define CLIP_XYZ_MODE 1                     // also clips basd on x,y,z.

#define FLAT_SHADING 1              // Each polygon has distinct shade for entire surface  
#define CONSTANT_SHADING 0
#define ONE_SIDED 0
#define TWO_SIDED 1             

#define RESET_POLY_LIST 0           // Resets polygon list by setting num_polys_frame = 0


// used to compute the min and max of two expresions
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (b) : (a))

// some math macros
#define DEG_TO_RAD(ang) ((ang)*M_PI/180)
#define RAD_TO_DEG(rads) ((rads)*180/M_PI)

// bit manipulation macros
#define SET_BIT(word,bit_flag) ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))

#endif