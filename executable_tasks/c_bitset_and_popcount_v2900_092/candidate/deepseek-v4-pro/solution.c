#include "interface.h"
#include <stdint.h>
#include <stddef.h>

static inline uint64_t popcount_64(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7FULL;
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_total = 0;
        for (size_t i = 0; i < word_count; ++i) {
            iter_total += popcount_64(left[i] & right[i]);
        }
        total = iter_total;
    }
    return total;
}