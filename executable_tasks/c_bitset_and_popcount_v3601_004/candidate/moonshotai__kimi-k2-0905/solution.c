#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcnt64(uint64_t x) { return (uint64_t)__builtin_popcountll(x); }

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
        size_t i = 0;
        for (; i + 3 < word_count; i += 4) {
            acc0 += popcnt64(left[i+0] & right[i+0]);
            acc1 += popcnt64(left[i+1] & right[i+1]);
            acc2 += popcnt64(left[i+2] & right[i+2]);
            acc3 += popcnt64(left[i+3] & right[i+3]);
        }
        total = acc0 + acc1 + acc2 + acc3;
        for (; i < word_count; ++i) {
            total += popcnt64(left[i] & right[i]);
        }
    }
    return total;
}