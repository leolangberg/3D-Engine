#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

/**
* Prints Matrix.
* Having incorrect %f or %d makes all printouts bugged.
*/
void matrix_print(const Matrix* m1) {
    for(int i = 0; i < ROW; i++) {
        printf("|");
        for(int j = 0; j < COL; j++) {
            printf(" %f", m1->matrix[i][j]);
        } printf(" |\n");
    } printf("\n");
}

/**
* Performs Addition of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_add(const Matrix* m1, const Matrix* m2) {
    Matrix* result = malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++)  {
        for(int j = 0; j < COL; j++) {
            result->matrix[i][j] = m1->matrix[i][j] + m2->matrix[i][j];
        }
    }
    return result;
}

/**
* Performs Subtraction of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_sub(const Matrix* m1, const Matrix* m2) {
    Matrix* result = malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            result->matrix[i][j] = m1->matrix[i][j] - m2->matrix[i][j];
        }
    }
    return result;
}

/**
* Performs Multiplication of 2 Matrices.
* @return pointer to new Matrix.
*/
Matrix* matrix_mul(const Matrix* m1, const Matrix* m2) {
    Matrix* result = malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            float element = 0;
            for(int k = 0; k < COL; k++) {
                element += (m1->matrix[i][k] * m2->matrix[k][j]);
            }
            result->matrix[i][j] = element;
        }
    }
    return result;
}

/**
* Transposes the Matrix, which swaps rows with columns (A -> A^T).
* @param m1 Matrix to be transposed.
*/
void matrix_transpose(Matrix* m1) {
    Matrix transpose;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            transpose.matrix[j][i] = m1->matrix[i][j];
        }
    }
    *m1 = transpose;
}

/**
* Creates a new Indentity Matrix instance.
* @return Identity Matrix.
*/
Matrix* matrix_create_identity_matrix() {
    Matrix* I = malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COL; j++)
        {   
            if(i == j) {
                I->matrix[i][j] = 1;
            } else {
                I->matrix[i][j] = 0;
            }
        }
    }
    return I;
}

/**
* Creates a 4x4 translation matrix used for performing (add and subtract) via
* matrix multiplication.
* Constructs a new identity matrix that then sets the [3][0-2] into 
* given vector.
*/
Matrix* matrix_create_translation_matrix(const Vector* v1) {
    Matrix* translation = matrix_create_identity_matrix();
    translation->matrix[3][0] = v1->x;
    translation->matrix[3][1] = v1->y;
    translation->matrix[3][2] = v1->z;
    return translation;
}

/**
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around z-vector. For 3D rotations specifc matrices have to be 
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix_z(float angle_radian) {
    Matrix* rotation = malloc(sizeof(Matrix));
    rotation->matrix[0][0] = cosf(angle_radian);
    rotation->matrix[0][1] = (-sinf(angle_radian));
    rotation->matrix[0][2] = 0;
    rotation->matrix[0][3] = 0;
    rotation->matrix[1][0] = sinf(angle_radian);
    rotation->matrix[1][1] = cosf(angle_radian);
    rotation->matrix[1][2] = 0;
    rotation->matrix[1][3] = 0;
    rotation->matrix[2][0] = 0;
    rotation->matrix[2][1] = 0;
    rotation->matrix[2][2] = 1;
    rotation->matrix[2][3] = 0;
    rotation->matrix[3][0] = 0;
    rotation->matrix[3][1] = 0;
    rotation->matrix[3][2] = 0;
    rotation->matrix[3][3] = 1;
    return rotation;
}

/**
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around x-vector. For 3D rotations specifc matrices have to be 
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix_x(float angle_radian) {
    Matrix* rotation = malloc(sizeof(Matrix));
    rotation->matrix[0][0] = 1;
    rotation->matrix[0][1] = 0;
    rotation->matrix[0][2] = 0;
    rotation->matrix[0][3] = 0;
    rotation->matrix[1][0] = 0;
    rotation->matrix[1][1] = cosf(angle_radian);
    rotation->matrix[1][2] = -sinf(angle_radian);
    rotation->matrix[1][3] = 0;
    rotation->matrix[2][0] = 0;
    rotation->matrix[2][1] = sinf(angle_radian);
    rotation->matrix[2][2] = cosf(angle_radian);
    rotation->matrix[2][3] = 0;
    rotation->matrix[3][0] = 0;
    rotation->matrix[3][1] = 0;
    rotation->matrix[3][2] = 0;
    rotation->matrix[3][3] = 1;
    return rotation;
}

/**
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around y-vector. For 3D rotations specifc matrices have to be 
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix_y(float angle_radian) {
    Matrix* rotation = malloc(sizeof(Matrix));
    rotation->matrix[0][0] = cosf(angle_radian);
    rotation->matrix[0][1] = 0;
    rotation->matrix[0][2] = sinf(angle_radian);
    rotation->matrix[0][3] = 0;
    rotation->matrix[1][0] = 0;
    rotation->matrix[1][1] = 1;
    rotation->matrix[1][2] = 0;
    rotation->matrix[1][3] = 0;
    rotation->matrix[2][0] = -sinf(angle_radian);
    rotation->matrix[2][1] = 0;
    rotation->matrix[2][2] = cosf(angle_radian);
    rotation->matrix[2][3] = 0;
    rotation->matrix[3][0] = 0;
    rotation->matrix[3][1] = 0;
    rotation->matrix[3][2] = 0;
    rotation->matrix[3][3] = 1;
    return rotation;
}


/**
* Compares 2 Matrices to see if they are equal.
* @return bool true or false.
*/
bool matrix_equals(const Matrix* m1, const Matrix* m2) {
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            if(m1->matrix[i][j] != m2->matrix[i][j]) {
                return false;
            }
        }
    }
    return true;
}

/**
* Collects specific row and returns it as a vector.
* @return vector of row.
*/
Vector* vector_from_matrix_col(const Matrix* m1, int col) {
    return vector_create(m1->matrix[0][col], m1->matrix[1][col], m1->matrix[2][col]);
}

/**
* Returns a Matrix representation of a vector.
* @return Matrix of vector.
*/
Vector* vector_from_matrix_row(const Matrix* m1, int row) {
    return vector_create(m1->matrix[row][0], m1->matrix[row][1], m1->matrix[row][2]);
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
    result->matrix[0][3] = 1;

    result->matrix[1][0] = 0;
    result->matrix[1][1] = 0;
    result->matrix[1][2] = 0;
    result->matrix[1][3] = 1;

    result->matrix[2][0] = 0;
    result->matrix[2][1] = 0;
    result->matrix[2][2] = 0;
    result->matrix[2][3] = 1;

    result->matrix[3][0] = 0;
    result->matrix[3][1] = 0;
    result->matrix[3][2] = 0;
    result->matrix[3][3] = 1;

    return result;
}

Vector* vector_matrix_mul(const Vector* v1, const Matrix* m1) {
    return vector_from_matrix_row(matrix_mul(vector_as_matrix(v1), m1), 0);
}


/**
* Takes an array of vectors and places them into a matrix.
* @param array_of_vectors list of vectors to be restructured as a matrix.
* @param array_length length of vector array.
*/
Matrix* vectors_as_matrix(const Vector** array_of_vectors, int array_length) {
    if(array_length > ROW) { return NULL; }
    Matrix* m = malloc(sizeof(Matrix));
    for(int i = 0; i < array_length; i++) 
    {
        m->matrix[i][0] = array_of_vectors[i]->x;
        m->matrix[i][1] = array_of_vectors[i]->y;
        m->matrix[i][2] = array_of_vectors[i]->z;
        m->matrix[i][3] = 1;
    }
    if(array_length < ROW) {
        for(int i = array_length; i < ROW; i++)
        {
            m->matrix[i][0] = 0;
            m->matrix[i][1] = 0;
            m->matrix[i][2] = 0;
            m->matrix[i][3] = 0;
        }
    }

    return m;
}

/**
* Scales all matrix values by said scale_factor.
* Translates vertices to origin.
* Performs matrix multiplication with diagonal scale factors.
* Translates vertices back to object.
* @param m1 matrix of vertices to scale.
* @param center center point on which to scale around.
* @param scale_factor scaling multiplier.
*/
void matrix_scale(Matrix* m1, Vector* center, float scale_factor) {
    Matrix* m2 = matrix_create_identity_matrix();
    m2->matrix[0][0] = scale_factor;
    m2->matrix[1][1] = scale_factor;
    m2->matrix[2][2] = scale_factor;

    Vector p_negate = *center;
    vector_negate(&p_negate);
    Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
    *m1 = *(matrix_mul(m1, translate_to_origin));  

    *m1 = *(matrix_mul(m1, m2)); 

    Matrix* translate_back = matrix_create_translation_matrix(center);
    *m1 = *(matrix_mul(m1, translate_back)); 

    free(m2);
    free(translate_to_origin);
    free(translate_back);

}

/**
* Copies given matrix.
*/
Matrix* matrix_copy(const Matrix* original) {
    Matrix* copy = (Matrix*)malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            copy->matrix[i][j] = original->matrix[i][j];
        }
    }
    return copy;
}


/**
* Creates a "Point At" matrix that basically defines the view space. The inverse of this matrix
* becomes the view space transformation.
*/
Matrix* matrix_point_at(const Vector* pos, Vector* direction, const Vector* up) {
    
    Vector* forward = vector_normalize(direction);
    Vector* scale = vector_scale(forward, vector_dot_product(up, forward));
    Vector* newUp = vector_sub(scale, up);
    Vector* newRight = vector_cross_product(newUp, forward);

    Matrix* pointAt = matrix_create_identity_matrix();
    pointAt->matrix[0][0] = newRight->x;
    pointAt->matrix[0][1] = newRight->y;
    pointAt->matrix[0][2] = newRight->z;
    pointAt->matrix[0][3] = 0;
    pointAt->matrix[1][0] = newUp->x;
    pointAt->matrix[1][1] = newUp->y;
    pointAt->matrix[1][2] = newUp->z;
    pointAt->matrix[1][3] = 0;
    pointAt->matrix[2][0] = forward->x;
    pointAt->matrix[2][1] = forward->y;
    pointAt->matrix[2][2] = forward->z;
    pointAt->matrix[2][3] = 0;
    pointAt->matrix[3][0] = pos->x;
    pointAt->matrix[3][1] = pos->y;
    pointAt->matrix[3][2] = pos->z;
    pointAt->matrix[3][3] = 1.0f; 

    free(forward);
    free(scale);
    free(newUp);
    free(newRight);

    return pointAt;
}

/**
* Direct hardwired inverse of the specific "Point At" matrix, which result the final
* View space "Look At" transformation matrix.
*/
Matrix* matrix_quick_lookat_inverse(const Matrix* pointAt) {

    Matrix* lookAt = matrix_create_identity_matrix();

    lookAt->matrix[0][0] = pointAt->matrix[0][0];
    lookAt->matrix[0][1] = pointAt->matrix[1][0];
    lookAt->matrix[0][2] = pointAt->matrix[2][0];
    lookAt->matrix[0][3] = 0;
    lookAt->matrix[1][0] = pointAt->matrix[0][1];
    lookAt->matrix[1][1] = pointAt->matrix[1][1];
    lookAt->matrix[1][2] = pointAt->matrix[2][1];
    lookAt->matrix[1][3] = 0;
    lookAt->matrix[2][0] = pointAt->matrix[0][2];
    lookAt->matrix[2][1] = pointAt->matrix[1][2];
    lookAt->matrix[2][2] = pointAt->matrix[2][2];
    lookAt->matrix[2][3] = 0;


    lookAt->matrix[3][0] = -(pointAt->matrix[3][0] * pointAt->matrix[0][0] + pointAt->matrix[3][1] * pointAt->matrix[1][0] + pointAt->matrix[3][2] * pointAt->matrix[2][0]);
    lookAt->matrix[3][1] = -(pointAt->matrix[3][0] * pointAt->matrix[0][1] + pointAt->matrix[3][1] * pointAt->matrix[1][1] + pointAt->matrix[3][2] * pointAt->matrix[2][1]);
    lookAt->matrix[3][2] = -(pointAt->matrix[3][0] * pointAt->matrix[0][2] + pointAt->matrix[3][1] * pointAt->matrix[1][2] + pointAt->matrix[3][2] * pointAt->matrix[2][2]);
    lookAt->matrix[3][3] = 1.0f;  

    return lookAt;
}




