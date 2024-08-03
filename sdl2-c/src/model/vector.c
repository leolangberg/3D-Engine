#include "vector.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/**
* Prints vector.
*/
void vector_print(const Vector* v1) {
    printf("| %f |\n", v1->x);
    printf("| %f |\n", v1->y);
    printf("| %f |\n", v1->z);
    printf("\n");
}

/**
* Create a new vector instance.
*/
Vector* vector_create(float x, float y, float z) {
    Vector* v = malloc(sizeof(Vector));
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

/**
* Carmack's Inverse Square root
* @return square root of number.
*/
float d_sqrt(float number) 
{
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

/**
* Finds the total length of the vector (magnitude & angle).
* @return length of vector.
*/
float vector_length(const Vector* v) {
    return d_sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

/**
* Adds 2 Vectors and returns the summed vector.
* @return sum vector.
*/
Vector* vector_add(const Vector* v1, const Vector* v2) {
    Vector* result = malloc(sizeof(Vector));
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
    result->z = v1->z + v2->z;
    return result;
}

/**
* Subtracts 2 Vectors and returns the difference vector.
* @param v1 start point.
* @param v2 end point.
* @return difference vector (v2 - v1).
*/
Vector* vector_sub(const Vector* v1, const Vector* v2) {
    Vector* result = malloc(sizeof(Vector));
    result->x = v2->x - v1->x;
    result->y = v2->y - v1->y;
    result->z = v2->z - v1->z;
    return result;
}

/**
* Multiplies a vector with a Scalar.
*/
Vector* vector_scale(Vector* v1, float scalar) {
    Vector* vec = malloc(sizeof(Vector));
    vec->x = v1->x * scalar;
    vec->y = v1->y * scalar;
    vec->z = v1->z * scalar;
    return vec;
}

/**
* Negates vector direction by multiplying each component with (-1).
*/
void vector_negate(Vector* v1) {
    Vector vn = *v1;
    vn.x = (-(v1->x));
    vn.y = (-(v1->y));
    vn.z = (-(v1->z));
    *v1 = vn;
}

/**
* Normalizes the vector (scale with magnutide of itself).
*/
Vector* vector_normalize(Vector* v1) {
    float magnitude = vector_length(v1);
    if (magnitude > 0) {
        return vector_scale(v1, (1 / magnitude));
    }
    return vector_create(NAN, NAN, NAN);
}

/**
* Computes the Dot product of 2 vectors (sum the product all axis).
* @returns dot product.
*/
float vector_dot_product(const Vector* v1, const Vector* v2) {
    return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

/**
* Computes Dot product and returns the result as a new vector.
*/
Vector* vector_dot_product_vector(const Vector* v1, const Vector* v2) {
    return vector_create((v1->x * v2->x), (v1->y * v2->y), (v1->z * v2->z));
}

/**
* Computes the Cross product of 2 vectors and returns the a new result vector.
* @return cross product vector.
*/
Vector* vector_cross_product(const Vector* v1, const Vector* v2) {
    Vector* result = malloc(sizeof(Vector));
    float i = ((v1->y * v2->z) - (v1->z * v2->y));
    float j = (-((v1->x * v2->z) - (v1->z * v2->x)));
    float k = ((v1->x * v2->y) - (v1->y * v2->x));
    result->x = i;
    result->y = j;
    result->z = k;
    return result;
}


/**
* Calculate slope of 2 vectors (2D).
*/
float vector_slope(const Vector* v1, const Vector* v2) {
    return ((v2->y - v1->y) / (v2->x - v1->x));
}


/**
* Copies given vector.
*/
Vector* vector_copy(const Vector* original) {
    Vector* copy = (Vector*)malloc(sizeof(Vector));
    copy->x = original->x;
    copy->y = original->y;
    copy->z = original->z;
    return copy;
}

/**
* Resets vector to all 0 values.
*/
Vector* vector_set_zero(Vector* v1) {
    v1->x = 0; v1->y = 0; v1->z = 0;
    return v1;
}


/**
* Determines (2D) intersection of 2 lines (4 points) using Bezier parameters (s, t).
* @return vector with intersection coordinates. 
*/
Vector* vector_intersect_2d(const Vector* v0, const Vector* v1, const Vector* w0, const Vector* w1) {
  
    Vector* s1 = vector_create((v1->x - v0->x), (v1->y - v0->y), 0);
    Vector* s2 = vector_create((w1->x - w0->x), (w1->y - w0->y), 0);

    float s, t;
    s = ((-s1->y) * (v0->x - w0->x) + s1->x * (v0->y - w0->y)) / ((-s2->x) * s1->y + s1->x * s2->y);
    t = (s2->x * (v0->x - w0->y) - s2->y * (v0->x - w0->x)) / ((-s2->x) * s1->y + s1->x * s2->y);

    // check that lines actually intersect.
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
       return vector_create((v0->x + (t * s1->x)), (v0->y + (t * s1->y)), 0);   
    } 
    return vector_create(NAN,NAN,NAN);
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

int vector_equals(const Vector *v1, const Vector *v2) {
    if((v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->y)) {
        return 1;
    }
    return 0;
}