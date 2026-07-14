#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total_popcount = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < word_count; ++i) {
            current_iter_sum += __builtin_popcountll(left[i] & right[i]);
        }
        total_popcount = current_iter_sum;
    }
    return total_popcount;
}