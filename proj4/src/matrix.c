#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Returns the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid. Note that the matrix is in row-major order.
 */
double get(matrix *mat, int row, int col) {
    int item = *(mat->data + row * mat->cols + col);
    return item;
}

/*
 * Sets the value at the given row and column to val. You may assume `row` and
 * `col` are valid. Note that the matrix is in row-major order.
 */
void set(matrix *mat, int row, int col, double val) {
    *(mat->data + row * mat->cols + col) = val;
}

/*
 * Allocates space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set to NULL to indicate that
 * this matrix is not a slice. You should also set `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    if(rows <= 0 || cols <=0) {
        return -1;
    }

    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    matrix *mp = NULL;
    mp = (matrix *)malloc(sizeof(matrix));
    if (mp == NULL) {
        return -2;
    }
    
    // 3. Allocate space for the matrix data, initializing all entries to be 0. Return -2 if allocating memory failed.
    mp->data = (double *)calloc(rows * cols, sizeof(double));
    if (mp->data == NULL) {
        return -2;
    }

    // 4. Set the number of rows and columns in the matrix struct according to the arguments provided.
    // 5. Set the `parent` field to NULL, since this matrix was not created from a slice.
    // 6. Set the `ref_cnt` field to 1.
    mp->rows = rows;
    mp->cols = cols;
    mp->parent = NULL;
    mp->ref_cnt = 1;

    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    // 8. Return 0 upon success.
    *mat = mp;
    return 0;
}

/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or that you free `mat->parent->data` if `mat` is the last existing slice of its parent matrix and its parent
 * matrix has no other references (including itself).
 */
void deallocate_matrix(matrix *mat) {
    // 1. If the matrix pointer `mat` is NULL, return.
    // 2. If `mat` has no parent: decrement its `ref_cnt` field by 1. If the `ref_cnt` field becomes 0, then free `mat` and its `data` field.
    // 3. Otherwise, recursively call `deallocate_matrix` on `mat`'s parent, then free `mat`.
    if (mat == NULL) {
        return;
    }
    
    if (mat->parent == NULL) {
        (mat->ref_cnt)--;
        if (mat->ref_cnt == 0) {
            free(mat->data);
            mat->data = NULL;
            free(mat);
            mat = NULL;
        }
    } else {
        deallocate_matrix(mat->parent);
        free(mat);
    }
}

/*
 * Allocates space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set to `from` to indicate this matrix is a slice of `from`
 * and the reference counter for `from` should be incremented. Lastly, do not forget to set the
 * matrix's row and column values as well.
 * You should return -1 if either `rows` or `cols` or both have invalid values. Return -2 if any
 * call to allocate memory in this function fails.
 * Return 0 upon success.
 * NOTE: Here we're allocating a matrix struct that refers to already allocated data, so
 * there is no need to allocate space for matrix data.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int offset, int rows, int cols) {
    // 1. Check if the dimensions are valid. Return -1 if either dimension is not positive.
    if (rows <= 0 || cols <= 0) {
        return -1;
    }
    // 2. Allocate space for the new matrix struct. Return -2 if allocating memory failed.
    matrix *mp = NULL;
    mp = (matrix *)malloc(sizeof(matrix));
    if (mp == NULL) {
        return -2;
    }
    // 3. Set the `data` field of the new struct to be the `data` field of the `from` struct plus `offset`.
    mp->data = from->data + offset;
    // 4. Set the number of rows and columns in the new struct according to the arguments provided.
    // 5. Set the `parent` field of the new struct to the `from` struct pointer.
    mp->rows = rows;
    mp->cols = cols;
    mp->parent = from;
    // 6. Increment the `ref_cnt` field of the `from` struct by 1.
    (from->ref_cnt)++;
    // 7. Store the address of the allocated matrix struct at the location `mat` is pointing at.
    *mat = mp;
    // 8. Return 0 upon success.
    return 0;
}















/*
 * Sets all entries in mat to val. Note that the matrix is in row-major order.
 */
void fill_matrix(matrix *mat, double val) {
    /* Normal implementation */
    // for (size_t i = 0; i < mat->rows; i++) {
    //     for (size_t j = 0; j < mat->cols; j++) {
    //         set(mat, i, j, val);
    //     }
    // }

    // SIMD implementation
    __m256d fill_vec = _mm256_set1_pd(val);
    size_t i = 0;
    for (; i < mat->rows * mat->cols / 4 * 4; i+=4) {
        _mm256_storeu_pd(mat->data + i, fill_vec);
    }

    //tail case
    for (; i < mat->rows * mat->cols; i++) {
        *(mat->data + i) = val;
    }
    
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /*  Normal implementation  */
    // double val = 0.0;
    // for (size_t i = 0; i < mat->rows; i++) {
    //     for (size_t j = 0; j < mat->cols; j++) {
    //         val = get(mat, i, j);
    //         if (val < 0) {
    //             val *= -1;
    //         }
    //         set(result, i, j, val);
    //     }
    // }

    /* SIMD */
    __m256d _zero = _mm256_set1_pd(0);
    size_t i = 0;
    for (; i < mat->rows * mat->cols / 4 * 4; i+=4) {
        __m256d mtx_vec = _mm256_loadu_pd(mat->data + i);
        __m256d sub_vec = _mm256_sub_pd(_zero, mtx_vec);
        __m256d abs_vec = _mm256_max_pd(sub_vec, mtx_vec);
        _mm256_storeu_pd(result->data + i, abs_vec);
    }

    // tail case
    for (; i < mat->rows * mat->cols; i++) {
        double val = *(mat->data + i);
        if (val < 0) {
            val *= -1;
        }
        *(result->data + i) = val;
    }

    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success.
 * Note that the matrix is in row-major order.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* Normal */
    // double val = 0.0;
    // for (size_t i = 0; i < mat->rows; i++) {
    //     for (size_t j = 0; j < mat->cols; j++) {
    //         val = get(mat, i, j) * (-1);
    //         set(result, i, j, val);
    //     }
    // }

    /* SIMD */
    __m256d _zero = _mm256_set1_pd(0);
    size_t i = 0;
    for (; i < mat->rows * mat->cols / 4 * 4; i+=4) {
        __m256d mtx_vec = _mm256_loadu_pd(mat->data + i);
        __m256d neg_vec = _mm256_sub_pd(_zero, mtx_vec);
        _mm256_storeu_pd(result->data + i, neg_vec);
    }
    
    // tail case
    for (; i < mat->rows * mat->cols; i++) {
        int val = *(mat->data + i) * (-1);
        *(result->data + i) = val;
    }

    return 0;
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* Normal */
    // double val = 0.0;
    // for (size_t i = 0; i < mat1->rows; i++) {
    //     for (size_t j = 0; j < mat1->cols; j++) {
    //         val = get(mat1, i, j) + get(mat2, i, j);
    //         set(result, i, j, val);
    //     }
    // }

    /* SIMD */
    size_t i = 0;
    for (; i < mat1->rows * mat1->cols / 4 * 4; i+=4) {
        __m256d vec1 = _mm256_loadu_pd(mat1->data + i);
        __m256d vec2 = _mm256_loadu_pd(mat2->data + i);
        __m256d add_vec = _mm256_add_pd(vec1, vec2);
        _mm256_storeu_pd(result->data + i, add_vec);
    }
    
    //tail case
    for (; i < mat1->rows * mat1->cols; i++) {
        *(result->data + i) = *(mat1->data + i) + *(mat2->data + i);
    }

    return 0;
}

/*
 * (OPTIONAL)
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success.
 * You may assume `mat1` and `mat2` have the same dimensions.
 * Note that the matrix is in row-major order.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* Normal */
    // double val = 0.0;
    // for (size_t i = 0; i < mat1->rows; i++) {
    //     for (size_t j = 0; j < mat1->cols; j++) {
    //         val = get(mat1, i, j) - get(mat2, i, j);
    //         set(result, i, j, val);
    //     }
    // }

    /* SIMD */
    size_t i = 0;
    for (; i < mat1->rows * mat1->cols / 4 * 4; i+=4) {
        __m256d vec1 = _mm256_loadu_pd(mat1->data + i);
        __m256d vec2 = _mm256_loadu_pd(mat2->data + i);
        __m256d sub_vec = _mm256_sub_pd(vec1, vec2);
        _mm256_storeu_pd(result->data + i, sub_vec);
    }
    
    //tail case
    for (; i < mat1->rows * mat1->cols; i++) {
        *(result->data + i) = *(mat1->data + i) - *(mat2->data + i);
    }

    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 * You may assume `mat1`'s number of columns is equal to `mat2`'s number of rows.
 * Note that the matrix is in row-major order.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* Normal */
    // for (size_t i = 0; i < mat1->rows; i++) {
    //     for (size_t j = 0; j < mat2->cols; j++) {
    //         double val = 0.0;
    //         for (size_t z = 0; z < mat1->cols; z++) {
    //             val += get(mat1, i, z) * get(mat2, z, j);
    //         }
    //         set(result, i, j, val);
    //     }
    // }

    /* SIMD */
    // transpose the 2nd matrix
    matrix *tmp = NULL;
    allocate_matrix(&tmp, mat2->cols, mat2->rows);
    for (size_t i = 0; i < mat2->rows; i++) {
        for (size_t j = 0; j < mat2->cols; j++) {
            set(tmp, j, i, get(mat2, i, j));
        }
    }
    
    // calculate the dot product
    double tmp_arr[4];
    for (size_t i = 0; i < mat1->rows; i++) {
        for (size_t j = 0; j < tmp->rows; j++) {
            // process 4 elements once
            double sum = 0.0;
            size_t z = 0;
            for (; z < tmp->cols / 4 * 4; z+=4) {
                __m256d vec1 = _mm256_loadu_pd(mat1->data + i * mat1->cols + z);
                __m256d vec2 = _mm256_loadu_pd(tmp->data + j * tmp->cols + z);
                __m256d mul_vec = _mm256_mul_pd(vec1, vec2);
                _mm256_storeu_pd(tmp_arr, mul_vec);
                sum += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3];
            }

            // tail case
            for (; z < tmp->cols; z++) {
                sum += get(mat1, i, z) * get(tmp, j, z);
            }

            set(result, i, j, sum);
        }
    }

    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 * You may assume `mat` is a square matrix and `pow` is a non-negative integer.
 * Note that the matrix is in row-major order.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    // create an identity matrix
    if (pow == 0) {
        for (size_t i = 0; i < result->rows; i++) {
            set(result, i, i, 1);
        } 
        return 0;
    }

    // create an all zero matrix
    matrix *zero = NULL;
    allocate_matrix(&zero, result->rows, result->rows);
    fill_matrix(zero, 0);
    add_matrix(result, zero, mat);
    if (pow == 1) {
        return 0;
    } 

    matrix *tmp = NULL;
    allocate_matrix(&tmp, result->rows, result->rows);
    for (size_t i = 0; i < pow - 1; i++) {
        mul_matrix(tmp, result, mat);
        add_matrix(result, zero, tmp);
    }
    return 0;
}
