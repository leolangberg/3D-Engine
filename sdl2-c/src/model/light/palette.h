#ifndef PALETTE_H
#define PALETTE_H

#define _RGB565FROM16BIT(RGB, r,g,b) { *b = ( ((RGB) >> 11) & 0x1f); *g = (((RGB) >> 5) & 0x3f); *r = ((RGB) & 0x1f); }
#define _RGB565FROM888(RGB, r, g, b) { *b = ( ((RGB) >> 16) & 0x1f); *g = (((RGB) >> 8) & 0x3f); *r = ((RGB) & 0x1f); }

// this builds a 32 bit color value in A.8.8.8 format (8-bit alpha mode)
#define _RGB32BIT(a,r,g,b) ((r) + ((g) << 8) + ((b) << 16) + ((a) << 24))
// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))

#define RGB_FROM_INT(value) ((RGB){ .rgba = (value) })
#define MAX_COLORS_PALETTE 256

/**
* The Idea is to have a main palette[256] array that then stores all possible
* 8-bit combinations in terms of colors. 
*/

typedef struct {
    unsigned char red;      // red channel 8-bit
    unsigned char green;    // green channel 8-bit
    unsigned char blue;     // blue channel 8-bit
    unsigned char flags;    // flags control.
} Palette;

// standard main array.
Palette standard_pal[MAX_COLORS_PALETTE];

// rgb color lookup array.
unsigned char rgblookup[65536];

// rgb intensity color lookup 2D array.
unsigned char rgbilookup[256][256];


// RGB + alhpa color
typedef struct { 
    union {
        int rgba;                   // compressed format
        unsigned char rgba_M[4];    // array format
        struct {
            unsigned char a, b, g, r; // explicit name format
        };
    };
}RGB;


// change a single color in the standard main color palette
int set_palette_entry(int color_index, Palette color);

// returns the given palette from standard based on index.
Palette* get_palette_entry(int color_index);

// function loads a palette from disk by iterating through all
// 256 slots and filling them in with color.
int Load_palette_from_file(char *filename, Palette* palette);

int Build_RGB_Lookup_Table(int rgb_format, Palette* src_palette, unsigned char* rgblookup);

int Build_RGB_Intensity_Lookup_Table(Palette* src_palette, unsigned char rgbilookup[256][256], int intensity_normalization);




#endif