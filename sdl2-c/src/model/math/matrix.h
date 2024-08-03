#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
#include <stdbool.h>

#define ROW 4
#define COL 4


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
* Creates a new Indentity Matrix instance.
* @return Identity Matrix.
*/
Matrix* matrix_create_identity_matrix();

/**
* Creates a 4x4 translation matrix used for performing (add and subtract) via
* matrix multiplication.
* Constructs a new identity matrix that then sets the [3][0-2] into 
* given vector.
*/
Matrix* matrix_create_translation_matrix(const Vector* v1);

/**
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around z-vector. For 3D rotations specifc matrices have to be 
* constructed for rotation along x, y axis aswell.
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix(float angle_radian);

Matrix* matrix_create_rotation_matrix_y(float angle_radian);
Matrix* matrix_create_rotation_matrix_x(float angle_radian);
Matrix* matrix_create_rotation_matrix_z(float angle_radian);


/**
* Compares 2 Matrices to see if they are equal.
* @return bool true or false.
*/
bool matrix_equals(const Matrix *m1, const Matrix *m2);

/**
* Collects specific column and returns it as a vector.
* @param col choose between columns 0-2.
* @return vector of column.
*/
Vector* vector_from_matrix_col(const Matrix* m1, int col);

/**
* Collects specific row and returns it as a vector.
* @param row choose between rows 0-2.
* @return vector of row.
*/
Vector* vector_from_matrix_row(const Matrix* m1, int row);

/**
* Returns a Matrix representation of a vector.
* @return Matrix of vector.
*/
Matrix* vector_as_matrix(const Vector* v1);

/**
* Takes an array of vectors and places them into a matrix.
* @param array_of_vectors list of vectors to be restructured as a matrix.
* @param array_length length of vector array.
*/
Matrix* vectors_as_matrix(const Vector** array_of_vectors, int array_length);

/**
* Performs matrix multiplication with vector that is temporarily restructured
* as a matrix.
*/
Vector* vector_matrix_mul(const Vector* v1, const Matrix* m1);

/**
* Scales all matrix values by said scale_factor.
* Translates vertices to origin.
* Performs matrix multiplication with diagonal scale factors.
* Translates vertices back to object.
* @param m1 matrix of vertices to scale.
* @param center center point on which to scale around.
* @param scale_factor scaling multiplier.
*/
void matrix_scale(Matrix* m1, Vector* center, float scale_factor);

/**
* Copies given matrix.
*/
Matrix* matrix_copy(const Matrix* original);

/**
* Creates a "Point At" matrix that basically defines the view space. The inverse of this matrix
* becomes the view space transformation.
*/
Matrix* matrix_point_at(const Vector* pos, Vector* direction, const Vector* up);

/**
* Direct hardwired inverse of the specific "Point At" matrix, which result the final
* View space "Look At" transformation matrix.
*/
Matrix* matrix_quick_lookat_inverse(const Matrix* pointAt);

#endif
