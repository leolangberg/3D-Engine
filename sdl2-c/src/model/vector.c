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
* @return difference vector.
*/
Vector* vector_sub(const Vector* v1, const Vector* v2) {
    Vector* result = malloc(sizeof(Vector));
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
    result->z = v1->z - v2->z;
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
* Returns a Matrix representation of a vector.
* @return Matrix of vector.
*/
Matrix* vector_as_matrix(const Vector* v1) {
    Matrix* result = malloc(sizeof(Matrix));
    result->matrix[0][0] = v1->x;
    result->matrix[0][1] = v1->y;
    result->matrix[0][2] = v1->z;
    return result;
}