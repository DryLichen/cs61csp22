#include <stdio.h>
#include "bit_ops.h"

/* Returns the Nth bit of X. Assumes 0 <= N <= 31. */
unsigned get_bit(unsigned x, unsigned n) {
    x << n;
    x >> 31;
    return x; /* UPDATE WITH THE CORRECT RETURN VALUE*/
}

/* Set the nth bit of the value of x to v. Assumes 0 <= N <= 31, and V is 0 or 1 */
void set_bit(unsigned *x, unsigned n, unsigned v) {
    *x
}

/* Flips the Nth bit in X. Assumes 0 <= N <= 31.*/
void flip_bit(unsigned *x, unsigned n) {
    
}

