
#ifndef GLOBAL_H
#define GLOBAL_H

/**
* Global constant declared.
* 
* Window measurements refer to pixel array.
* Screen measurements mean actual screen proportions.
*/

#define SCREENWIDTH 1280            
#define SCREENHEIGHT 720

#define WINDOW_WIDTH 384
#define WINDOW_HEIGHT 216
#define ALL_PIXELS 82944

#define VIEWING_DISTANCE 250
#define ASPECT_RATIO 1

#define CLIP_FAR_Z 1000.0f
#define CLIP_NEAR_Z 1.0f
#define poly_clip_max_x WINDOW_WIDTH - 1
#define poly_clip_max_y WINDOW_HEIGHT - 1
#define poly_clip_min_x 0
#define poly_clip_min_y 0

#define OBJECT_CULL_Z_MODE   0
#define OBJECT_CULL_XYZ_MODE 1
#define INVERSE_ASPECT_RATIO 1

#define RESET_POLY_LIST 0

#define CLIP_Z_MODE 0
#define CLIP_XYZ_MODE 1


#endif