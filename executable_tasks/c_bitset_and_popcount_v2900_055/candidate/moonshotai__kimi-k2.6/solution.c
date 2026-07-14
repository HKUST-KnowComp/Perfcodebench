#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 0;
        size_t i = 0;
        for (; i + 7 < word_count; i += 8) {
            c0 += (uint64_t)__builtin_popcountll(left[i]     & right[i]);
            c1 += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
            c2 += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
            c3 += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
            c4 += (uint64_t)__builtin_popcountll(left[i + 4] & right[i + 4]);
            c5 += (uint64_t)__builtin_popcountll(left[i + 5] & right[i + 5]);
            c6 += (uint64_t)__builtin_popcountll(left[i + 6] & right[i + 6]);
            c7 += (uint64_t)__builtin_popcountll(left[i + 7] & right[i + 7]);
        }
        total = c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7;
        for (; i < word_count; ++i) {
            total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
        }
    }
    return total;
}
