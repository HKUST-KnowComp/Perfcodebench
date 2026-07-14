#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (iters <= 0) {
        return 0;
    }

    size_t i = 0;
    uint64_t a0 = 0, a1 = 0, a2 = 0, a3 = 0;

    for (; i + 8 <= word_count; i += 8) {
        a0 += (uint64_t)__builtin_popcountll(left[i]     & right[i]);
        a1 += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
        a2 += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
        a3 += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
        a0 += (uint64_t)__builtin_popcountll(left[i + 4] & right[i + 4]);
        a1 += (uint64_t)__builtin_popcountll(left[i + 5] & right[i + 5]);
        a2 += (uint64_t)__builtin_popcountll(left[i + 6] & right[i + 6]);
        a3 += (uint64_t)__builtin_popcountll(left[i + 7] & right[i + 7]);
    }

    uint64_t total = a0 + a1 + a2 + a3;

    for (; i < word_count; ++i) {
        total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }

    return total;
}
