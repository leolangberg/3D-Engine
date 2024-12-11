#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

/**
* Carmack's Inverse Square root
* @return square root of number. */
float d_sqrt(float number)  {
    int i;
    float x,y;
    x = number * 0.5;
    y = number;
    i = *(int*) &y;
    i = 0x5f3759df - (i >> 1);
    y = * (float*) &i;
    y = y * (1.5 - (x * y * y));
    y = y * (1.5 - (x * y * y));
    return number * y;
}

// prints vector.
void vector_print(const Vector* v1) {
    printf("| %f |\n| %f |\n| %f |\n\n", v1->x, v1->y, v1->z);
}

// Normalizes the vector (scale with magnutide of itself).
Vector vector_normalize(const Vector* v1) {
    float magnitude = vector_length(v1);
    if(magnitude < 0) { printf("magnitude < 0 somehow.\n"); }
    return vector_scale(v1, (1.0 / magnitude));
}