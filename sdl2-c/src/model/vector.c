#include "vector.h"
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
void vector_scale(Vector* v1, float scalar) {
    v1->x = v1->x * scalar;
    v1->y = v1->y * scalar;
    v1->z = v1->z * scalar;
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
void vector_normalize(Vector* v1) {
    float magnitude = vector_length(v1);
    if (magnitude > 0) {
        vector_scale(v1, (1 / magnitude));
    }
}

/**
* Computes the Dot product of 2 vectors (sum the product all axis).
* @returns dot product.
*/
float vector_dot_product(const Vector* v1, const Vector* v2) {
    return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
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



