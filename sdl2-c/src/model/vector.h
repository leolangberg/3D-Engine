#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

/*
* Vector structure (3D).
* Holds x,y,z values (float).
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
void vector_normalize(Vector* v1);

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
* Determines (2D) intersection of 2 lines (4 points) using Bezier parameters (s, t).
* @return vector with intersection coordinates. 
*/
Vector* vector_intersect_2d(const Vector* v0, const Vector* v1, const Vector* w0, const Vector* w1);

Vector* get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y);

#endif