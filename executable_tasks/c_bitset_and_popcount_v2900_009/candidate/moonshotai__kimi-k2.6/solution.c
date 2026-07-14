#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t acc0 = 0;
    uint64_t acc1 = 0;
    uint64_t acc2 = 0;
    uint64_t acc3 = 0;

    size_t i = 0;
    const size_t n = word_count;

    for (; i + 3 < n; i += 4) {
        acc0 += (uint64_t)__builtin_popcountll(left[i]     & right[i]);
        acc1 += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
        acc2 += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
        acc3 += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
    }

    for (; i < n; ++i) {
        acc0 += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }

    return acc0 + acc1 + acc2 + acc3;
}
