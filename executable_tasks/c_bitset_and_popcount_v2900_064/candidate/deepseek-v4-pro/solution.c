#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcount64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(x);
#else
    const uint64_t m1  = 0x5555555555555555ULL;
    const uint64_t m2  = 0x3333333333333333ULL;
    const uint64_t m4  = 0x0f0f0f0f0f0f0f0fULL;
    const uint64_t h01 = 0x0101010101010101ULL;
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < word_count; ++i) {
            sum += popcount64(left[i] & right[i]);
        }
        total = sum;
    }
    return total;
}
