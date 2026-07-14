#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    if (iters <= 0) return 0;

    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t i = 0;
    
    for (; i + 3 < word_count; i += 4) {
        sum0 += (uint64_t)__builtin_popcountll(left[i] & right[i]);
        sum1 += (uint64_t)__builtin_popcountll(left[i+1] & right[i+1]);
        sum2 += (uint64_t)__builtin_popcountll(left[i+2] & right[i+2]);
        sum3 += (uint64_t)__builtin_popcountll(left[i+3] & right[i+3]);
    }
    
    uint64_t sum = sum0 + sum1 + sum2 + sum3;
    for (; i < word_count; ++i) {
        sum += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }

    return sum;
}
