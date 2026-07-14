#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t final_total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t t0 = 0, t1 = 0, t2 = 0, t3 = 0;
        uint64_t t4 = 0, t5 = 0, t6 = 0, t7 = 0;
        size_t i = 0;
        for (; i + 7 < word_count; i += 8) {
            t0 += __builtin_popcountll(left[i] & right[i]);
            t1 += __builtin_popcountll(left[i+1] & right[i+1]);
            t2 += __builtin_popcountll(left[i+2] & right[i+2]);
            t3 += __builtin_popcountll(left[i+3] & right[i+3]);
            t4 += __builtin_popcountll(left[i+4] & right[i+4]);
            t5 += __builtin_popcountll(left[i+5] & right[i+5]);
            t6 += __builtin_popcountll(left[i+6] & right[i+6]);
            t7 += __builtin_popcountll(left[i+7] & right[i+7]);
        }
        for (; i < word_count; ++i) {
            t0 += __builtin_popcountll(left[i] & right[i]);
        }
        final_total = t0 + t1 + t2 + t3 + t4 + t5 + t6 + t7;
    }
    return final_total;
}
