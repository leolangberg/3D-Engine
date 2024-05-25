#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "vector.h"

#define ROW 4
#define COL 4

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
    for(int i = 0; i < ROW; i++) 
    {
        for(int j = 0; j < COL; j++)
        {
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
    for(int i = 0; i < ROW; i++) 
    {
        for(int j = 0; j < COL; j++)
        {
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
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COL; j++)
        {
            float element = 0;
            for(int k = 0; k < COL; k++)
            {
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
* Rotates matrix with angle given.
* 
* Calls matrix_mul and then dereferences *result, which
* is then placed on the dereference *m1, thus changing
* the inside variable that m1 points to.
* @param m1 Matrix to be rotated.
* @param point_of_rotation vector on which the rotation will be performed.
* @param angle_radian angle of rotation (radians).
*/
void matrix_rotate(Matrix* m1, Vector* point_of_rotation, float angle_radian) {
    Vector p_negate = *point_of_rotation;
    vector_negate(&p_negate);
    Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
    *m1 = *(matrix_mul(m1, translate_to_origin));  

    Matrix* rot_z = matrix_create_rotation_matrix_z(angle_radian);
    Matrix* rot_y = matrix_create_rotation_matrix_y(angle_radian);
    Matrix* rot_x = matrix_create_rotation_matrix_x(angle_radian);
    *m1 = *(matrix_mul(m1, rot_z)); 
    *m1 = *(matrix_mul(m1, rot_y)); 
    *m1 = *(matrix_mul(m1, rot_x)); 

    Matrix* translate_back = matrix_create_translation_matrix(point_of_rotation);
    *m1 = *(matrix_mul(m1, translate_back)); 

    free(rot_z);
    free(rot_y);
    free(rot_x);
    free(translate_to_origin);
    free(translate_back);
}

/**
* Rotates matrix along z axis only.
*/
void matrix_rotate_z(Matrix* m1, Vector* point_of_rotation, float angle_radian) {
    Vector p_negate = *point_of_rotation;
    vector_negate(&p_negate);
    Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
    *m1 = *(matrix_mul(m1, translate_to_origin));  

    Matrix* rot_z = matrix_create_rotation_matrix_z(angle_radian);
    *m1 = *(matrix_mul(m1, rot_z)); 

    Matrix* translate_back = matrix_create_translation_matrix(point_of_rotation);
    *m1 = *(matrix_mul(m1, translate_back)); 

    free(rot_z);
    free(translate_to_origin);
    free(translate_back);
}

/**
* Rotates matrix along y axis only.
*/
void matrix_rotate_y(Matrix* m1, Vector* point_of_rotation, float angle_radian) {
    Vector p_negate = *point_of_rotation;
    vector_negate(&p_negate);
    Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
    *m1 = *(matrix_mul(m1, translate_to_origin));  

    Matrix* rot_y = matrix_create_rotation_matrix_y(angle_radian);
    *m1 = *(matrix_mul(m1, rot_y)); 

    Matrix* translate_back = matrix_create_translation_matrix(point_of_rotation);
    *m1 = *(matrix_mul(m1, translate_back)); 

    free(rot_y);
    free(translate_to_origin);
    free(translate_back);
}

/**
* Rotates matrix along x axis only.
*/
void matrix_rotate_x(Matrix* m1, Vector* point_of_rotation, float angle_radian) {
    Vector p_negate = *point_of_rotation;
    vector_negate(&p_negate);
    Matrix* translate_to_origin = matrix_create_translation_matrix(&p_negate);
    *m1 = *(matrix_mul(m1, translate_to_origin));  

    Matrix* rot_x = matrix_create_rotation_matrix_x(angle_radian);
    *m1 = *(matrix_mul(m1, rot_x)); 

    Matrix* translate_back = matrix_create_translation_matrix(point_of_rotation);
    *m1 = *(matrix_mul(m1, translate_back)); 

    free(rot_x);
    free(translate_to_origin);
    free(translate_back);
}


/**
* Swaps 2 rows inside matrix.
*
* First checks so that Row numbers are valid.
* Dynamically allocates temporary m1 (dereference so values are copied, not shared).
*/
static void matrix_swap_rows(Matrix* m1, int row1, int row2) {
    if(row1 < 0 || row1 > ROW) { return; }
    if(row2 < 0 || row2 > ROW) { return; }

    Matrix* tmp_m1 = malloc(sizeof(Matrix));
    *tmp_m1 = *m1;

    for(int i = 0; i < COL; i++) {
        m1->matrix[row1][i] = m1->matrix[row2][i];
        m1->matrix[row2][i] = tmp_m1->matrix[row1][i];
    }
    free(tmp_m1);
}

/**
* Multiplies all values within a specifed row with a given factor.
*/
static void matrix_multiply_row(Matrix* m1, int row1, float factor) {
    if(row1 < 0 || row1 > ROW) { return; }
    for(int i = 0; i < COL; i++) {
        m1->matrix[row1][i] = (factor * (m1->matrix[row1][i]));
    }
}

/**
* Adds one rows onto another.
* @param row1 row that is to be added.
* @param row2 reciever that is to be added onto (sum).
*/
static void matrix_add_row(Matrix* m1, int row1, int row2) {
    if(row1 < 0 || row1 > ROW) { return; }
    if(row2 < 0 || row2 > ROW) { return; }

    for(int i = 0; i < COL; i++) {
        m1->matrix[row2][i] += m1->matrix[row1][i];
    }
}

/**
* Multiplies all values within a specifed row with a given factor.
* Then adds that row onto another row inside the matrix.
* Finally multiplies reverse factor to get back to previous state for row1.
* @param row1 row to be multiplied.
* @param row2 row on which the addition will be applied.
* @param factor multiplication variable.
*/
static void matrix_multiply_row_add(Matrix* m1, int row1, int row2, float factor) {
    matrix_multiply_row(m1, row1, factor);
    matrix_add_row(m1, row1, row2);
    matrix_multiply_row(m1, row1, (1 / factor));
}

/**
* Retrieves row with maximum element at given column.
*/
static int matrix_find_row_with_column_max_value(const Matrix* m1, int col1) {
    int row = 0;
    float num = m1->matrix[row][col1];
    for(int i = 1; i < ROW; i++) {
        if(m1->matrix[i][col1] > num) {
            num = m1->matrix[i][col1];
            row = i; 
        }
    }
    return row;   
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
* Performs columnwise Gauss elimination.
* 
* This is the method that performs addition and subtraction of columns
* onto the Matrix A and B eqiuvalently.
*
*/
static void matrix_gauss_jordan_column_operation(Matrix* m1, Matrix* m2, int cRow, int cCol) {
    int rowIndex = cRow + 1;
    while(rowIndex < ROW)
    {
        if(m1->matrix[rowIndex][cCol] != 0)
        {
            int rowOneIndex = cCol;
            float rowOneValue = m1->matrix[rowOneIndex][cCol];

            if(rowOneValue != 0) {
                float cElementValue = m1->matrix[rowIndex][cCol];
                float cElementValue2 = m2->matrix[rowIndex][cCol];
                float correctionFactor = (-(cElementValue / rowOneValue));
                float correctionFactor2 = (-(cElementValue2 / rowOneValue));

                for(int i = 0; i < COL; i++)
                {
                    m1->matrix[rowIndex][i] += (m1->matrix[rowOneIndex][i] * correctionFactor);
                    m2->matrix[rowIndex][i] += (m2->matrix[rowOneIndex][i] * correctionFactor2); //correctionfactor2 here?
                }
            }
        } 
        rowIndex++;
    }
}

/**
* Performs Rowwise Gauss elimination.
* 
* This is the method that performs addition and subtraction of rows
* onto the Matrix A and B eqiuvalently.
*/
static void matrix_gauss_jordan_row_operation(Matrix* m1, Matrix* m2, int cRow, int cCol) {
    int colIndex = cCol + 1;
    while(colIndex < COL)
    {
        if(m1->matrix[cRow][colIndex] != 0)
        {
            int rowOneIndex = colIndex;
            float rowOneValue = m1->matrix[rowOneIndex][colIndex];

            /**
            * Cannot divide by 0.
            */
            if(rowOneValue != 0) {
                float cElementValue = m1->matrix[cRow][colIndex];
                float cElementValue2 = m1->matrix[cRow][colIndex];

                float correctionFactor = (-(cElementValue / rowOneValue));
                float correctionFactor2 = (-(cElementValue2 / rowOneValue));

                /**
                * Performs the Row addition from one row onto another.
                */
                for(int i = 0; i < ROW; i++)
                {
                    m1->matrix[cRow][i] += (m1->matrix[rowOneIndex][i] * correctionFactor);
                    m2->matrix[cRow][i] += (m2->matrix[rowOneIndex][i] * correctionFactor2); //correctionfactor2 here?
                }
            }
        } 
        colIndex++;
    }
}

/**
* Gauss Jordan Elimination Algorithm.
*
* Performs row and column operations until Matrix A equals Identity Matrix (I).
* Once this occurs then Matrix B has been properly transformed to correct x,y,z values.
*/
void matrix_gauss_jordan_elimination(Matrix* A, Matrix* B) {
    Matrix* m1 = A;
    Matrix* m2 = B;
    Matrix* I = matrix_create_identity_matrix();

    int errorCounter = 0;
    int diagIndex = 0;
    int cRow, cCol;

    /**
    * Iterates until Matrix A is equal to Indentity Matrix (I)
    * meaning that A is in row echelon form and B is corrected.
    */
    while(matrix_equals(m1, I) != true)
    {
        diagIndex = 0;

        /**
        * Loop attempts to reorder Matrix so that rows with the highest values
        * for said column becomes placed on top, which helps in creating a
        * row echelon form.
        */
        while(diagIndex < ROW)
        {
            cRow = diagIndex;
            cCol = diagIndex;
            int row_with_max_elem = matrix_find_row_with_column_max_value(m1, cCol);

            /**
            * If highest value found for said column is not in the current row
            * then a swap is performed to place higher value rows higher up the matrix.
            */
            if(row_with_max_elem != cRow)
            {   
                matrix_swap_rows(m1, cRow, row_with_max_elem);
                matrix_swap_rows(B, cRow, row_with_max_elem);
            }
            /**
            * If the diagonal current position does not equal to 1 then 
            * division is made for the whole row to scale until the current diagonal 
            * number is 1 (row multiplication). 
            */
            if(m1->matrix[cRow][cCol] != 1) 
            {   
                float target = m1->matrix[cRow][cCol];
                for(int i = 0; i < COL; i++) {
                    m1->matrix[cRow][i] = (m1->matrix[cRow][i] * (1 / target));
                    m2->matrix[cRow][i] = (m2->matrix[cRow][i] * (1 / target));
                }
            } 
            /**
            * Once rearrangement of Matrix has taken place the column and row operations
            * are called, which is where the actual Gauss Elimination occurs.
            */
            matrix_gauss_jordan_column_operation(m1, m2, cRow, cCol);
            matrix_gauss_jordan_row_operation(m1, m2, cRow, cCol);
            diagIndex++;
        }
        errorCounter++;
        if(errorCounter > 10) { return; }
    }
    *A = *m1;
    *B = *m2;
}

/**
* Algorithm for finding the Matrix Inverse.
* 
* Uses Gauss-Jordan Elimination then retrieves the new Inverse 
* to the Matrix A.
*/
void matrix_inverse(Matrix* A) {
    Matrix* m1 = A;
    Matrix* inverse = matrix_create_identity_matrix();
    matrix_gauss_jordan_elimination(m1, inverse);
    *A = *inverse;
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
    return result;
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
* Applies perspective transformation on matrix.
* First performs matrix multiplication which incorporates (d) distance into
* matrix. 
* Each row x,y,z values are then divided by their equivalent d/z value.
*/
void matrix_perspective_transformation(Matrix* m1, float distance) {
    if(distance == 0) { return; }
    Matrix* perspective = matrix_create_identity_matrix();
    perspective->matrix[2][3] = 1 / distance;
    perspective->matrix[3][3] = 0;
    Matrix* tmp = matrix_mul(m1, perspective);

    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COL; j++)
        {
            tmp->matrix[i][j] = (tmp->matrix[i][j] / tmp->matrix[i][3]);
        }
    }
    *m1 = *tmp;
}

/**
* Copies given matrix.
*/
Matrix* matrix_copy(const Matrix* original) {
    Matrix* copy = (Matrix*)malloc(sizeof(Matrix));
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COL; j++)
        {
            copy->matrix[i][j] = original->matrix[i][j];
        }
    }
    return copy;
}




