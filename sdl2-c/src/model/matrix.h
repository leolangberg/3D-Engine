#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

#define ROW 3
#define COL 3


/**
* Matrix struct.
* Holds 2D array of Rows and Columns.
*/
typedef struct{
    float matrix[ROW][COL];
}Matrix;

/**
* Prints Matrix.
* Having incorrect %f or %d makes all printouts bugged.
*/
void matrix_print(const Matrix* m);

/**
* Performs Addition of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_add(const Matrix* m1, const Matrix* m2);

/**
* Performs Subtraction of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_sub(const Matrix* m1, const Matrix* m2);

/**
* Performs Multiplication of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_mul(const Matrix* m1, const Matrix* m2);

/**
* Transposes the Matrix, which swaps rows with columns (A -> A^T).
* @param m1 Matrix to be transposed.
*/
void matrix_transpose(Matrix *m);

/**
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around z-vector. For 3D rotations specifc matrices have to be 
* constructed for rotation along x, y axis aswell.
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix(float angle_radian);

/**
* Rotates matrix with angle given.
* 
* Calls matrix_mul and then dereferences *result, which
* is then placed on the dereference *m1, thus changing
* the inside variable that m1 points to.
* @param m1 Matrix to be rotated.
* @param angle_radian angle of rotation (radians).
*/
void matrix_rotate(Matrix *m, float angle_radian);

/**
* Creates a new Indentity Matrix instance.
* @return Identity Matrix.
*/
Matrix* matrix_create_identity_matrix();

/**
* Compares 2 Matrices to see if they are equal.
* @return bool true or false.
*/
bool matrix_equals(const Matrix *m1, const Matrix *m2);

/**
* Gauss Jordan Elimination Algorithm.
*
* Performs row and column operations until Matrix A equals Identity Matrix (I).
* Once this occurs then Matrix B has been properly transformed to correct x,y,z values.
*/
void matrix_gauss_jordan_elimination(Matrix* A, Matrix* B);

/**
* Algorithm for finding the Matrix Inverse.
* 
* Uses Gauss-Jordan Elimination then retrieves the new Inverse 
* to the Matrix A.
*/
void matrix_inverse(Matrix* A);




#endif
