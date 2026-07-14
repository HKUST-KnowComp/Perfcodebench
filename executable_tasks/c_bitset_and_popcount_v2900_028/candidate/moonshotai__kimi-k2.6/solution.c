#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcount64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return (uint64_t)__builtin_popcountll(x);
#else
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint64_t* restrict l = left;
        const uint64_t* restrict r = right;
        size_t n = word_count;

        uint64_t sum0 = 0;
        uint64_t sum1 = 0;
        uint64_t sum2 = 0;
        uint64_t sum3 = 0;

        for (; n >= 4; n -= 4, l += 4, r += 4) {
            sum0 += popcount64(l[0] & r[0]);
            sum1 += popcount64(l[1] & r[1]);
            sum2 += popcount64(l[2] & r[2]);
            sum3 += popcount64(l[3] & r[3]);
        }

        uint64_t sum = sum0 + sum1 + sum2 + sum3;

        for (; n > 0; --n, ++l, ++r) {
            sum += popcount64(*l & *r);
        }

        total = sum;
    }

    return total;
}
