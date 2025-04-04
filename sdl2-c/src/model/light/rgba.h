#ifndef RGBA_H
#define RGBA_H

#include <stdio.h>
#include "../camera.h"
#include "../object/polygon.h"
#include "../math/vector.h"

typedef struct {
    unsigned char a, b, g, r;
}RGBA;

void Load_palette(RGBA *palette, const int pal_length, const char *filename);

void light(Object *object, RGBA *palette, Vector *light_source, float ambient_light);

#endif