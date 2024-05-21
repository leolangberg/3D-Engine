#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include "matrix.h"

/*
* Vector structure (3D).
*/
typedef struct{
    float x;
    float y;
    float z;
}Vector;

/**
* Prints vector.
*/
void vector_print(const Vector* v1);

/**
* Carmack's Inverse Square root
* @return square root of number.
*/
float d_sqrt(float number);

/**
* Finds the total length of the vector (magnitude & angle).
* @return length of vector.
*/
float vector_length(const Vector* v);

/**
* Adds 2 Vectors and returns the summed vector.
* @return sum vector.
*/
Vector* vector_add(const Vector* v1, const Vector* v2);

/**
* Subtracts 2 Vectors and returns the difference vector.
* @return difference vector.
*/
Vector* vector_sub(const Vector* v1, const Vector* v2);

/**
* Multiplies a vector with a Scalar.
*/
void vector_scale(Vector* v1, float scalar);

/**
* Normalizes the vector (scale with magnutide of itself).
*/
void vector_normalize(Vector* v1);

/**
* Computes the Dot product of 2 vectors (sum the product all axis).
* @returns dot product.
*/
float vector_dot_product(const Vector* v1, const Vector* v2);

/**
* Computes the Cross product of 2 vectors and returns the a new result vector.
* @return cross product vector.
*/
Vector* vector_cross_product(const Vector* v1, const Vector* v2);

/**
* Returns a Matrix representation of a vector.
* @return Matrix of vector.
*/
Matrix* vector_as_matrix(const Vector* v1);


#endif