#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcount64(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
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
