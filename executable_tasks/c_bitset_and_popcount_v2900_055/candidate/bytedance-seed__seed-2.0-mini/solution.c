#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (word_count == 0 || iters <= 0) {
        return 0;
    }
    uint64_t single_sum = 0;
    for (size_t i = 0; i < word_count; ++i) {
        single_sum += __builtin_popcountll(left[i] & right[i]);
    }
    return single_sum * (uint64_t)iters;
}