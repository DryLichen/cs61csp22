#include <stdio.h>
#include "bit_ops.h"

/* Returns the Nth bit of X. Assumes 0 <= N <= 31. */
/* 可以直接写二进制，比如0b011001 */
unsigned get_bit(unsigned x, unsigned n) {
    x = x >> n;
    return (x & 1);
}

/* Set the nth bit of the value of x to v. Assumes 0 <= N <= 31, and V is 0 or 1 */
void set_bit(unsigned *x, unsigned n, unsigned v) {
    *x &= ~(1 << n);
    *x |= (v << n);
}

/* Flips the Nth bit in X. Assumes 0 <= N <= 31.*/
void flip_bit(unsigned *x, unsigned n) {
    unsigned y = get_bit(*x, n);
    set_bit(x, n, ~y&1);

    // unsigned y = 1 << n;
    // *x = *x ^ y
}

