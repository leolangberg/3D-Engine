#ifndef VECTOR_H
#define VECTOR_H

// Vector structure (3D).
// Holds x,y,z values (float).
typedef struct{
    float x, y, z;
}Vector;

// Carmack's Inverse Square root.
float d_sqrt(float number);

// prints vector.
void vector_print(const Vector* v1);

// Finds the total length of the vector (magnitude & angle).
static inline float vector_length(const Vector* v) {
    return d_sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

// Creates new vector instance.
static inline Vector vector_create(float x, float y, float z) {
    return (Vector) {x,y,z};
}

// Adds 2 Vectors and returns the summed vector.
static inline Vector vector_add(const Vector* v1, const Vector* v2) {
    return (Vector) {(v1->x + v2->x), (v1->y + v2->y), (v1->z + v2->z)};
}

// Subtracts 2 Vectors and returns the difference vector.
// v1 is start point and v2 is end point. Therefore returns vector (v2 - v1).
static inline Vector vector_sub(const Vector* v1, const Vector* v2) {
    return (Vector) {(v2->x - v1->x), (v2->y - v1->y), (v2->z - v1->z)};
}

// Multiplies a vector with a Scalar.
static inline Vector vector_scale(const Vector* v1, const float scalar) {
    return (Vector) {(v1->x * scalar), (v1->y * scalar), (v1->z * scalar)};
}

// Negates vector direction by multiplying each component with (-1).
static inline Vector vector_negate(const Vector* v1) {
    return (Vector) {(-v1->x), (-v1->y), (-v1->z)};
}

// Computes the Dot product of 2 vectors (sum the product of all axis).
static inline float vector_dot_product(const Vector* v1, const Vector* v2) {
    return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

// Computes Dot product and returns the result as a new vector.
static inline Vector vector_dot_product_vector(const Vector* v1, const Vector* v2) {
    return vector_create((v1->x * v2->x), (v1->y * v2->y), (v1->z * v2->z));
}

// Calculate slope of 2 vectors (2D).
static inline float vector_slope(const Vector* v1, const Vector* v2) {
    return ((v2->y - v1->y) / (v2->x - v1->x));
}

// Copies given vector.
static inline Vector vector_copy(const Vector* original) {
    return (Vector) {original->x, original->y, original->z};
}

// Computes the Cross product of 2 vectors and returns the a new result vector.
static inline Vector vector_cross_product(const Vector* v1, const Vector* v2) {
    float i = ((v1->y * v2->z)   - (v1->z * v2->y));
    float j = (-((v1->x * v2->z) - (v1->z * v2->x)));
    float k = ((v1->x * v2->y)   - (v1->y * v2->x));
    return vector_create(i, j, k);
}

// Compare 2 vectors and determine if they are the equal.
static inline int vector_equals(const Vector *v1, const Vector *v2) {
    if((v1->x == v2->x) && (v1->y == v2->y) && (v1->z == v2->y)) {
        return 1;
    }
    return 0;
}

// Normalizes the vector (scale with magnutide of itself).
Vector vector_normalize(const Vector* v1);

#endif