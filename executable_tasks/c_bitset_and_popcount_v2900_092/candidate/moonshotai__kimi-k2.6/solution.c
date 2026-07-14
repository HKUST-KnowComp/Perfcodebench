#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint64_t* l = left;
        const uint64_t* r = right;
        size_t n = word_count;
        uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

        while (n >= 4) {
            sum0 += (uint64_t)__builtin_popcountll(l[0] & r[0]);
            sum1 += (uint64_t)__builtin_popcountll(l[1] & r[1]);
            sum2 += (uint64_t)__builtin_popcountll(l[2] & r[2]);
            sum3 += (uint64_t)__builtin_popcountll(l[3] & r[3]);
            l += 4;
            r += 4;
            n -= 4;
        }

        total = sum0 + sum1 + sum2 + sum3;
        while (n--) {
            total += (uint64_t)__builtin_popcountll(*l++ & *r++);
        }
    }
    return total;
}
