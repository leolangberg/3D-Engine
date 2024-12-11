#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"

#define ROW 4
#define COL 4

// Matrix struct, holds 2D array of Rows and Columns.
typedef struct{
    float matrix[ROW][COL];
}Matrix;

// Prints Matrix.
void matrix_print(const Matrix* m);

// Performs Addition of 2 Matrices.
Matrix matrix_add(const Matrix* m1, const Matrix* m2);

// Performs Subtraction of 2 Matrices.
Matrix matrix_sub(const Matrix* m1, const Matrix* m2);

// Performs Multiplication of 2 Matrices.
Matrix matrix_mul(const Matrix* m1, const Matrix* m2);

// Transposes the Matrix, which swaps rows with columns (A -> A^T).
Matrix matrix_transpose(const Matrix *m);

// Creates a new Indentity Matrix instance.
Matrix matrix_create_identity_matrix(void);

// Creates a 4x4 translation matrix used for performing (add and subtract) via
// matrix multiplication. Constructs a new identity matrix that then sets 
// the [3][0-2] into given vector.
Matrix matrix_create_translation_matrix(const Vector* v1);

// Creates a new Rotation Matrix instance based of an angle.
// Rotation around z-vector. For 3D rotations specifc matrices have to be 
// constructed for rotation along x, y axis aswell.
Matrix matrix_create_rotation_matrix_y(float angle_radian);
Matrix matrix_create_rotation_matrix_x(float angle_radian);
Matrix matrix_create_rotation_matrix_z(float angle_radian);

// Compares 2 Matrices to see if they are equal.
int matrix_equals(const Matrix *m1, const Matrix *m2);

// Returns a Matrix representation of a vector.
Matrix vector_as_matrix(const Vector* v1);

// Takes an array of vectors and places them into a matrix.
// array_of_vectors represents list of vectors to be restructured as a matrix.
// array_length represents length of vector array.
Matrix vectors_as_matrix(const Vector** array_of_vectors, int array_length);

// Scales all matrix values by said scale_factor and translates vertices to origin.
// Performs matrix multiplication with diagonal scale factors and finally 
// translates vertices back to object.
Matrix matrix_scale(Matrix* m1, const Vector* center, float scale_factor);

// Copies given matrix.
Matrix matrix_copy(const Matrix* original);

// Creates a "Point At" matrix that basically defines the view space. 
// The inverse of this matrix becomes the view space transformation.
Matrix matrix_point_at(const Vector* pos, Vector* direction, const Vector* up);

// Direct hardwired inverse of the specific "Point At" matrix, 
// which result the final view space "Look At" transformation matrix.
Matrix matrix_quick_lookat_inverse(const Matrix* pointAt);

// Collects specific column and returns it as a vector.
static inline Vector vector_from_matrix_col(const Matrix* m1, int col) {
    return vector_create(m1->matrix[0][col], m1->matrix[1][col], m1->matrix[2][col]);
}

// Returns a Matrix representation of a vector.
static inline Vector vector_from_matrix_row(const Matrix* m1, int row) {
    return vector_create(m1->matrix[row][0], m1->matrix[row][1], m1->matrix[row][2]);
}

// Performs matrix multiplication with vector that is temporarily restructured as a matrix.
Vector vector_matrix_mul(const Vector* v1, const Matrix* m1);

#endif