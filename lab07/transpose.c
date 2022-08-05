#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* column major matrix */
/* Here, blocksize means the size of integer block, not cache */
/* Implement cache blocking below. You should NOT assume that n is a multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
    // YOUR CODE HERE
    int chunk_num = n / blocksize;

    // normal case
    for (int i = 0; i < chunk_num; i++) {
        for (int j = 0; j < chunk_num; j++) {
            for (int x = 0; x < blocksize; x++) {
                for (int y = 0; y < blocksize; y++) {
                    dst[y + j * blocksize + (x + i * blocksize) * n] = src[x + i * blocksize + (y + j * blocksize) * n];
                }
            }
        }
    }

    // If n is not a multiple of blocksize, handle the edge case
    int normalsize = chunk_num * blocksize;
    for (int k = 0; k < n; k++) {
        for (int z = normalsize; z < n; z++) {
            dst[z + k * n] = src[k + z * n];
            dst[k + z * n] = src[z + k * n];
        }
    } 
}
