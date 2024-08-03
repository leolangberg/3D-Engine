#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

/*
* Vector structure (3D).
* Holds x,y,z values (float).
*/
typedef struct{
    float x, y, z;
}Vector;

/**
* Prints vector.
*/
void vector_print(const Vector* v1);

/**
* Creates a new vector instance.
*/
Vector* vector_create(float x, float y, float z);

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
* @param v1 start point.
* @param v2 end point.
* @return difference vector (v2 - v1).
*/
Vector* vector_sub(const Vector* v1, const Vector* v2);

/**
* Multiplies a vector with a Scalar.
*/
Vector* vector_scale(Vector* v1, float scalar);

/**
* Negates vector direction by multiplying each component with (-1).
*/
void vector_negate(Vector* v1);

/**
* Normalizes the vector (scale with magnutide of itself).
*/
Vector* vector_normalize(Vector* v1);

/**
* Computes the Dot product of 2 vectors (sum the product all axis).
* @returns dot product.
*/
float vector_dot_product(const Vector* v1, const Vector* v2);

/**
* Computes Dot product and returns the result as a new vector.
*/
Vector* vector_dot_product_vector(const Vector* v1, const Vector* v2);

/**
* Computes the Cross product of 2 vectors and returns the a new result vector.
* @return cross product vector.
*/
Vector* vector_cross_product(const Vector* v1, const Vector* v2);

/**
* Calculate slope of 2 vectors (2D).
*/
float vector_slope(const Vector* v1, const Vector* v2);

/**
* Copies given vector.
*/
Vector* vector_copy(const Vector* original);

/**
* Resets vector to all 0 values.
*/
Vector* vector_set_zero(Vector* v1);

/**
* this function computes the intersection fo the sent lines
* and returns the intersection point, note that the function assumes
* the lines intersect. The function can handle vertical as well as
* horizontal lines. Note the function isnt very clever, it simply applies
* the math, but we dont need speed since this si a pre-processing step.
*/
void intersect_lines(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float *xi, float *yi);

/**
* Returns 1 if vectors are exactly identical (float) level.
*/
int vector_equals(const Vector *v1, const Vector *v2);

#endif