#include "polygon.h"



Triangle triangle_create(const Vector* v1, const Vector* v2, const Vector* v3) {
    Triangle triangle;
    triangle.v[0] = *v1;
    triangle.v[1] = *v2;
    triangle.v[2] = *v2;
    return triangle;
}