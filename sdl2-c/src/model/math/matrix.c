#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"


// Prints Matrix.
// Having incorrect %f or %d makes all printouts bugged.
void matrix_print(const Matrix* m1) {
    for(int i = 0; i < ROW; i++) {
        printf("|");
        for(int j = 0; j < COL; j++) {
            printf(" %f", m1->matrix[i][j]);
        } printf(" |\n");
    } printf("\n");
}

// Performs Addition of 2 Matrices.
Matrix matrix_add(const Matrix* m1, const Matrix* m2) {
    Matrix result;
    for(int i = 0; i < ROW; i++)  {
        for(int j = 0; j < COL; j++) {
            result.matrix[i][j] = m1->matrix[i][j] + m2->matrix[i][j];
        }
    }
    return result;
}

// Performs Subtraction of 2 Matrices.
Matrix matrix_sub(const Matrix* m1, const Matrix* m2) {
    Matrix result;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            result.matrix[i][j] = m1->matrix[i][j] - m2->matrix[i][j];
        }
    }
    return result;
}

// Performs Multiplication of 2 Matrices.
Matrix matrix_mul(const Matrix* m1, const Matrix* m2) {
    Matrix result;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            float element = 0;
            for(int k = 0; k < COL; k++) {
                element += (m1->matrix[i][k] * m2->matrix[k][j]);
            }
            result.matrix[i][j] = element;
        }
    }
    return result;
}

// Transposes the Matrix, which swaps rows with columns (A -> A^T).
Matrix matrix_transpose(const Matrix* m1) {
    Matrix transpose;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            transpose.matrix[j][i] = m1->matrix[i][j];
        }
    }
    return transpose;
}

// Creates a new Indentity Matrix instance.
Matrix matrix_create_identity_matrix(void) {
    Matrix I;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {   
            if(i == j) { I.matrix[i][j] = 1;} 
            else       { I.matrix[i][j] = 0; }
        }
    }
    return I;
}

// Creates a 4x4 translation matrix used for performing (add and subtract) via
// matrix multiplication. Constructs a new identity matrix that then sets 
// the [3][0-2] into given vector.
Matrix matrix_create_translation_matrix(const Vector* v1) {
    Matrix translation = matrix_create_identity_matrix();
    translation.matrix[3][0] = v1->x;
    translation.matrix[3][1] = v1->y;
    translation.matrix[3][2] = v1->z;
    return translation;
}

// Creates a new Rotation Matrix instance based of an angle.
// Rotation around z-vector. For 3D rotations specifc matrices have to be 
Matrix matrix_create_rotation_matrix_z(float angle_radian) {
    Matrix rotation;
    rotation.matrix[0][0] = cosf(angle_radian);
    rotation.matrix[0][1] = (-sinf(angle_radian));
    rotation.matrix[0][2] = 0;
    rotation.matrix[0][3] = 0;
    rotation.matrix[1][0] = sinf(angle_radian);
    rotation.matrix[1][1] = cosf(angle_radian);
    rotation.matrix[1][2] = 0;
    rotation.matrix[1][3] = 0;
    rotation.matrix[2][0] = 0;
    rotation.matrix[2][1] = 0;
    rotation.matrix[2][2] = 1;
    rotation.matrix[2][3] = 0;
    rotation.matrix[3][0] = 0;
    rotation.matrix[3][1] = 0;
    rotation.matrix[3][2] = 0;
    rotation.matrix[3][3] = 1;
    return rotation;
}

// Creates a new Rotation Matrix instance based of an angle.
// Rotation around x-vector. For 3D rotations specifc matrices have to be 
Matrix matrix_create_rotation_matrix_x(float angle_radian) {
    Matrix rotation;
    rotation.matrix[0][0] = 1;
    rotation.matrix[0][1] = 0;
    rotation.matrix[0][2] = 0;
    rotation.matrix[0][3] = 0;
    rotation.matrix[1][0] = 0;
    rotation.matrix[1][1] = cosf(angle_radian);
    rotation.matrix[1][2] = -sinf(angle_radian);
    rotation.matrix[1][3] = 0;
    rotation.matrix[2][0] = 0;
    rotation.matrix[2][1] = sinf(angle_radian);
    rotation.matrix[2][2] = cosf(angle_radian);
    rotation.matrix[2][3] = 0;
    rotation.matrix[3][0] = 0;
    rotation.matrix[3][1] = 0;
    rotation.matrix[3][2] = 0;
    rotation.matrix[3][3] = 1;
    return rotation;
}

// Creates a new Rotation Matrix instance based of an angle.
// Rotation around y-vector. For 3D rotations specifc matrices have to be 
Matrix matrix_create_rotation_matrix_y(float angle_radian) {
    Matrix rotation;
    rotation.matrix[0][0] = cosf(angle_radian);
    rotation.matrix[0][1] = 0;
    rotation.matrix[0][2] = sinf(angle_radian);
    rotation.matrix[0][3] = 0;
    rotation.matrix[1][0] = 0;
    rotation.matrix[1][1] = 1;
    rotation.matrix[1][2] = 0;
    rotation.matrix[1][3] = 0;
    rotation.matrix[2][0] = -sinf(angle_radian);
    rotation.matrix[2][1] = 0;
    rotation.matrix[2][2] = cosf(angle_radian);
    rotation.matrix[2][3] = 0;
    rotation.matrix[3][0] = 0;
    rotation.matrix[3][1] = 0;
    rotation.matrix[3][2] = 0;
    rotation.matrix[3][3] = 1;
    return rotation;
}


// Compares 2 Matrices to see if they are equal.
int matrix_equals(const Matrix* m1, const Matrix* m2) {
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            if(m1->matrix[i][j] != m2->matrix[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

// Returns a Matrix representation of a vector.
Matrix vector_as_matrix(const Vector* v1) {
    Matrix result;
    result.matrix[0][0] = v1->x;
    result.matrix[0][1] = v1->y;
    result.matrix[0][2] = v1->z;
    result.matrix[0][3] = 1;

    result.matrix[1][0] = 0;
    result.matrix[1][1] = 0;
    result.matrix[1][2] = 0;
    result.matrix[1][3] = 1;

    result.matrix[2][0] = 0;
    result.matrix[2][1] = 0;
    result.matrix[2][2] = 0;
    result.matrix[2][3] = 1;

    result.matrix[3][0] = 0;
    result.matrix[3][1] = 0;
    result.matrix[3][2] = 0;
    result.matrix[3][3] = 1;
    return result;
}

// Takes an array of vectors and places them into a matrix.
// array_of_vectors represents list of vectors to be restructured as a matrix.
// array_length represents length of vector array.
Matrix vectors_as_matrix(const Vector** array_of_vectors, int array_length) {
    if(array_length > ROW) { perror("vectors_as_matrix: array_length > ROW"); }
    Matrix m;
    for(int i = 0; i < array_length; i++)  {
        m.matrix[i][0] = array_of_vectors[i]->x;
        m.matrix[i][1] = array_of_vectors[i]->y;
        m.matrix[i][2] = array_of_vectors[i]->z;
        m.matrix[i][3] = 1;
    }
    if(array_length < ROW) {
        for(int i = array_length; i < ROW; i++) {
            m.matrix[i][0] = 0;
            m.matrix[i][1] = 0;
            m.matrix[i][2] = 0;
            m.matrix[i][3] = 0;
        }
    }
    return m;
}

// Scales all matrix values by said scale_factor and translates vertices to origin.
// Performs matrix multiplication with diagonal scale factors and finally 
// translates vertices back to object.
Matrix matrix_scale(Matrix* m1, const Vector* center, float scale_factor) {
    Matrix result, 
           m2 = matrix_create_identity_matrix();
    m2.matrix[0][0] = scale_factor;
    m2.matrix[1][1] = scale_factor;
    m2.matrix[2][2] = scale_factor;

    Vector negate = vector_negate(center);
    Matrix translate_to_origin = matrix_create_translation_matrix(&negate);
    
    *m1 = matrix_mul(m1, &translate_to_origin);  
    *m1 = matrix_mul(m1, &m2); 

    Matrix translate_back = matrix_create_translation_matrix(center);
    result = matrix_mul(m1, &translate_back); 
    return result;
}

// Copies given matrix.
Matrix matrix_copy(const Matrix* original) {
    Matrix copy;
    for(int i = 0; i < ROW; i++) {
        for(int j = 0; j < COL; j++) {
            copy.matrix[i][j] = original->matrix[i][j];
        }
    }
    return copy;
}

// Creates a "Point At" matrix that basically defines the view space. 
// The inverse of this matrix becomes the view space transformation.
Matrix matrix_point_at(const Vector* pos, Vector* direction, const Vector* up) {
    Matrix pointAt = matrix_create_identity_matrix();
    Vector forward  = vector_normalize(direction);
    Vector scale    = vector_scale(&forward, vector_dot_product(up, &forward));
    Vector newUp    = vector_sub(&scale, up);
    Vector newRight = vector_cross_product(&newUp, &forward);

    pointAt.matrix[0][0] = newRight.x;
    pointAt.matrix[0][1] = newRight.y;
    pointAt.matrix[0][2] = newRight.z;
    pointAt.matrix[0][3] = 0;
    pointAt.matrix[1][0] = newUp.x;
    pointAt.matrix[1][1] = newUp.y;
    pointAt.matrix[1][2] = newUp.z;
    pointAt.matrix[1][3] = 0;
    pointAt.matrix[2][0] = forward.x;
    pointAt.matrix[2][1] = forward.y;
    pointAt.matrix[2][2] = forward.z;
    pointAt.matrix[2][3] = 0;
    pointAt.matrix[3][0] = pos->x;
    pointAt.matrix[3][1] = pos->y;
    pointAt.matrix[3][2] = pos->z;
    pointAt.matrix[3][3] = 1.0f; 
    return pointAt;
}

//  Direct hardwired inverse of the specific "Point At" matrix,
//  which result the final view space "Look At" transformation matrix.
Matrix matrix_quick_lookat_inverse(const Matrix* pointAt) {
    Matrix lookAt = matrix_create_identity_matrix();
    const float 
    m00 = pointAt->matrix[0][0],
    m01 = pointAt->matrix[0][1],
    m02 = pointAt->matrix[0][2],
    m10 = pointAt->matrix[1][0],
    m11 = pointAt->matrix[1][1],
    m12 = pointAt->matrix[1][2],
    m20 = pointAt->matrix[2][0],
    m21 = pointAt->matrix[2][1],
    m22 = pointAt->matrix[2][2],
    m30 = pointAt->matrix[3][0],
    m31 = pointAt->matrix[3][1],
    m32 = pointAt->matrix[3][2];
    
    lookAt.matrix[0][0] = m00;
    lookAt.matrix[0][1] = m10;
    lookAt.matrix[0][2] = m20;
    lookAt.matrix[0][3] = 0;
    lookAt.matrix[1][0] = m01;
    lookAt.matrix[1][1] = m11;
    lookAt.matrix[1][2] = m21;
    lookAt.matrix[1][3] = 0;
    lookAt.matrix[2][0] = m02;
    lookAt.matrix[2][1] = m12;
    lookAt.matrix[2][2] = m22;
    lookAt.matrix[2][3] = 0;

    lookAt.matrix[3][0] = -(m30*m00 + m31*m10 + m32*m20);
    lookAt.matrix[3][1] = -(m30*m01 + m31*m11 + m32*m21);
    lookAt.matrix[3][2] = -(m30*m02 + m31*m12 + m32*m22);
    lookAt.matrix[3][3] = 1.0f;  
    return lookAt;
}


// multiplies a vector with a matrix and then returns a new vector.
Vector vector_matrix_mul(const Vector* v1, const Matrix* m1) {
    Matrix m_vec = vector_as_matrix(v1);
    Matrix m_mul = matrix_mul(&m_vec, m1);
    return vector_from_matrix_row(&m_mul, 0);
}



