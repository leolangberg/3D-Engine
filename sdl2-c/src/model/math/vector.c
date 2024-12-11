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

void intersect_lines(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float *xi, float *yi) {
    // this function computes the intersection fo the sent lines
    // and returns the intersection point, note that the function assumes
    // the lines intersect. The function can handle vertical as well as
    // horizontal lines. Note the function isnt very clever, it simply applies
    // the math, but we dont need speed since this si a pre-processing step.

    float a1, b1, c1,       // constants of linear equations
          a2, b2, c2,
          det_inv,          // the inverse of the determinant of the coefficient matrix.
          m1, m2;           // the slopes of each line

    // compute slopes, note the cludge for infinity, however, this will
    // be close enough

    if((x1 - x0) != 0) {
        m1 = (y1 - y0) / (x1 - x0);
    }
    else {
        m1 = (float)1e+10; // close enough to infinity
    }

    if((x3 - x2) != 0) {
        m2 = (y3 - y2) / (x3 - x2);
    }
    else {
        m2 = (float)1e+10;  // close enough to infinity
    }

    // compute constants
    a1 = m1;
    a2 = m2;
    b1 = -1;
    b2 = -1;

    c1 = (y0 - m1*x0);
    c2 = (y2 - m2*x2);

    // compute the inverse of the determinant
    det_inv = 1 / (a1*b2 - a2*b1);

    // use Cramers rule to compute xi and yi
    *xi = ((b1*c2 - b2*c1) * det_inv);
    *yi = ((a2*c1 - a1*c2) * det_inv);
}

int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x;
    float s1_y;
    float s2_x;
    float s2_y;
    s1_x = p1_x - p0_x;     
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     
    s2_y = p3_y - p2_y;

    float s;
    float t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL) {
            *i_x = p0_x + (t * s1_x);
        }
        if (i_y != NULL) {
            *i_y = p0_y + (t * s1_y);
        }
    
        return 1;
    }

    return 0; // No collision
}



