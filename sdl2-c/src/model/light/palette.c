#include "palette.h"
#include <stdio.h>
#include <stdlib.h>
#include "../math/vector.h"
#include "../../integration/plgreader.h"


// change a single color in the standard main color palette
int set_palette_entry(int color_index, Palette color) {
    
    // make sure color index is not above or below 0..255
    if(color_index < 0 || color_index > 255) {
        return 0;
    }

    standard_pal[color_index] = color;
    return 1;
}

// returns the given palette from standard based on index.
Palette* get_palette_entry(int color_index) {
    return &standard_pal[color_index];
}

// function loads a palette from disk by iterating through all
// 256 slots and filling them in with color.
int Load_palette_from_file(char *filename, Palette* palette) {

    FILE *fp_file;      // working file
    int index,          // looping variable
        r,g,b,f;        // red, green, blue, flags
    
    char buffer[80];

    // try and open file
    if((fp_file = fopen(filename, "r")) == NULL) {
        printf("could not open file: %s\n", filename);
        return 0;
    }

    // loop through file
    for(index = 0; index < MAX_COLORS_PALETTE; index++)
    {
        // read the next entry
        if(!PLG_Get_Line(buffer, 80, fp_file)) {
            printf("Error with PLG file %s (stop 1)", filename);
            fclose(fp_file);
            return 0;
        }

        sscanf(buffer, "%d %d %d %d", &r, &g, &b, &f);

        palette[index].red   = r;
        palette[index].green = g;
        palette[index].blue  = b;
        palette[index].flags = f;
    }

    // close the file
    fclose(fp_file);

    // return sucess
    return 1;
}

/**
* Create a lookup table that in total consists of 16-bit (5,6,5) instead of the 8-bit channels, which
* combined becomes 8 + 8 + 8 = 24-bit colors. (8-bits per channel).
*/
int Build_RGB_Lookup_Table(int rgb_format, Palette* src_palette, unsigned char* rgblookup) {
    // this function takes as input the rgb format
    // hat it should generate the looup table for.
    // Function uses a simple least squares scan for all
    // possible RGB colors in the 16-bit color space and maps
    // that to the discrete RGB space in the 8-bit palette.

    int r,g,b;
    long delta_red,
         delta_green,
         delta_blue,
         error;

    if(!src_palette || !rgblookup) {
        return 0;
    }

    // there are total of 64k entries,
    // perform a loop and look them up, do the least
    // amount of work. There are 65536 * 256 iterations.
    for(int rgb_index = 0; rgb_index < 65536; rgb_index++)
    {
        int curr_index = -1;    // current color index of best match
        long curr_error = 2147483647; // distance in color space to nearest match or "error"

        for(int color_index = 0; color_index < 256; color_index++)
        {
            // extract r,g,b from rgb_index
            // assuming an encoding of 5.6.5, then scale to 8.8.8 since 
            // palette is in that format always
            r = ((rgb_index >> 11) << 3);
            g = ((rgb_index >> 5) & 0x3f) << 2;
            b = (rgb_index & 0x1f) << 3;

            // compute distance to color from target.
            delta_red   = abs(src_palette[color_index].red - r);
            delta_green = abs(src_palette[color_index].green - g);
            delta_blue  = abs(src_palette[color_index].blue - b);
            error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

            // is this color a better match?
            if(error < curr_error)
            {
                curr_index = color_index;
                curr_error = error;
            }
        } // end for color_index

        // best match has been found, enter it into table
        rgblookup[rgb_index] = curr_index;
    }
    return 1;
}

int Build_RGB_Intensity_Lookup_Table(Palette* src_palette, unsigned char rgbilookup[256][256], int intensity_normalization) {
    // this function takes the source palette to compute the 
    // intensity shading table with the table will be formatted
    // such that each row is a color index, and each column is 
    // the shade 0..255 desired, teh output is a single byte index.

    // The function builds the table by looping through each color
    // in the color palette and then for each color, it scales the 
    // color ot maximum intenisty wihtout overflow in the RGB channels.
    // It then uses this as the 100% intensity value of the color,
    // then the algorithm computes the 256 shades of the color, and 
    // then uses the starndard least squares scan to find the colors
    // in hte palette and stores them in the row of the current color
    // under intensity translation.

    // Note: if you set intensity normalization to 0 the maximazation
    // step is not performed.

    int ri, gi, bi,     // initial color
        rw, gw, bw;     // current working color

    float ratio,        // scaling ratio
          dl, dr,       // intensity gradients for 256 shades
          dg, db;

    // first check the pointers
    if(!src_palette || !rgbilookup) {
        return 0;
    }

    // for each color in the palette, compute maximum intensity value then scan
    // for 256 shades of it
    for(int col_index = 0; col_index < 256; col_index++)
    {
        // extract color from palette
        ri = src_palette[col_index].red;
        gi = src_palette[col_index].green;
        bi = src_palette[col_index].blue;

        // find largest channel then max it out and scale other
        // channels based on ratio
        if(intensity_normalization == 1)
        {
            // red largest?
            if(ri >= gi && ri >= bi)
            {
                // compute scaling ratio
                ratio = (float) 255 / (float) ri;

                // max colors out
                ri = 255;
                gi = (int)((float) gi * ratio + 0.5);
                bi = (int)((float) bi * ratio + 0.5);
            }
            else // green largest?
            if(gi >= ri && gi >= bi)
            {
                // compute scaling ratio
                ratio = (float) 255 / (float) gi;

                // max colors out
                gi = 255;
                ri = (int)((float) ri * ratio + 0.5);
                bi = (int)((float) bi * ratio + 0.5);
            }
            else // blue is largest 
            {
                ratio = (float) 255 / (float) bi;

                // max colors out
                bi = 255;
                ri = (int)((float) ri * ratio + 0.5);
                gi = (int)((float) gi * ratio + 0.5);
            }
        } // end if norm = 1

        // at this point, we need to compute the intesity gradients for this color,
        // so we can compute the RGB value for 256 shades of the current color
        dl = d_sqrt(ri*ri + gi*gi + bi*bi) / (float) 256;
        dr = ri / dl;
        dg = gi / dl;
        db = bi / dl;

        // initalize working color
        rw = 0;
        gw = 0;
        bw = 0;

        // at this point rw,gw,bw is the color that we need to ocmpute
        // the 256 intensities for to enter into the col_index (th) row
        // of the table.
        for(int intensity_index = 0; intensity_index < 256; intensity_index++)
        {
            int curr_index = -1;    // current color index of best match
            long curr_error = 2147483647; // distance in color space to nearest match or "error"

            for(int color_index = 0; color_index < 256; color_index++)
            {
                // compute distance to color from target
                long delta_red   = abs(src_palette[color_index].red - rw);
                long delta_green = abs(src_palette[color_index].green - gw);
                long delta_blue  = abs(src_palette[color_index].blue - bw);
                long error = (delta_red * delta_red) + (delta_green * delta_green) + (delta_blue * delta_blue);

                if(error < curr_error)
                {
                    curr_index = color_index;
                    curr_error = error;
                }
            } // end for loop color-index

            // best match has been found enter it into table
            rgbilookup[col_index][intensity_index] = curr_index;

            // compute next intensity level
            // (test for overflow, shouldnt happen, but never know)
            if(rw += dr < 255) {rw = 255; }
            if(gw += dg > 255) {gw = 255; }
            if(bw += db > 255) {bw = 255; }

        } // end for intensity index

    } // end for col_index

    return 1;
}