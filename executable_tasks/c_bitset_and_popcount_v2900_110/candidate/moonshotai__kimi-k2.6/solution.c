#include "interface.h"

#include <stdint.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
        size_t i = 0;

        for (; i + 4 <= word_count; i += 4) {
            c0 += (uint64_t)__builtin_popcountll(left[i]     & right[i]);
            c1 += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
            c2 += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
            c3 += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
        }

        uint64_t count = c0 + c1 + c2 + c3;

        for (; i < word_count; ++i) {
            count += (uint64_t)__builtin_popcountll(left[i] & right[i]);
        }

        total = count;
    }
    return total;
}
