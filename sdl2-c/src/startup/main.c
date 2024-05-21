#include <stdio.h>
#include "../matrix.h"
#include "../vector.h"

int main() {
    Matrix m1 = {
        {{1,2,-1},
         {-2,0,1},
         {1,-1,0}}
    };
    matrix_print(&m1); 

    Matrix m2 = {
        {{1,0,0},
         {1,0,0},
         {0,0,0}}
    };

    matrix_print(&m2);


    Vector v = {1,1,0};
    vector_normalize(&v);
    vector_print(&v);
    float len = vector_length(&v);

    return 0;
}

