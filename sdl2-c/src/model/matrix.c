#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

#define ROW 3
#define COL 3


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
* Creates a new Rotation Matrix instance based of an angle.
* 
* Rotation around z-vector. For 3D rotations specifc matrices have to be 
* constructed for rotation along x, y axis aswell.
* @param angle_radian angle of rotation (in radians).
* @return Rotational Matrix.
*/
Matrix* matrix_create_rotation_matrix(float angle_radian) {
    Matrix* rotation = malloc(sizeof(Matrix));
    rotation->matrix[0][0] = cosf(angle_radian);
    rotation->matrix[0][1] = (-sinf(angle_radian));
    //rotation->matrix[0][2] = 0;
    rotation->matrix[1][0] = sinf(angle_radian);
    rotation->matrix[1][1] = cosf(angle_radian);
    //rotation->matrix[1][2] = 0;
    //rotation->matrix[2][0] = 0;
    //rotation->matrix[2][1] = 0;
    rotation->matrix[2][2] = 1;
    return rotation;
}

/**
* Rotates matrix with angle given.
* 
* Calls matrix_mul and then dereferences *result, which
* is then placed on the dereference *m1, thus changing
* the inside variable that m1 points to.
* @param m1 Matrix to be rotated.
* @param angle_radian angle of rotation (radians).
*/
void matrix_rotate(Matrix* m1, float angle_radian) {
    Matrix* rot = matrix_create_rotation_matrix(angle_radian);
    *m1 = *(matrix_mul(m1, rot));  
}

/**
* Creates a new Indentity Matrix instance.
* @return Identity Matrix.
*/
Matrix* matrix_create_identity_matrix() {
    Matrix* I = malloc(sizeof(Matrix));
    I->matrix[0][0] = 1;
    I->matrix[1][1] = 1;
    I->matrix[2][2] = 1;
    return I;
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




