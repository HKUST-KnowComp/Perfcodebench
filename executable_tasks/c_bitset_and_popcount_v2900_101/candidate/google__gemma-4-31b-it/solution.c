#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total_popcount = 0;
    
    // The original code reset 'total' to 0 inside the iters loop, 
    // meaning it only actually returned the result of the final iteration.
    // However, the goal is to count set bits in the AND of two packed bitsets.
    // Based on the baseline's logic: total = 0; for(...) total += bits[i];
    // it effectively calculates the popcount of (left & right) for the last iteration.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_popcount = 0;
        for (size_t i = 0; i < word_count; ++i) {
            current_iter_popcount += __builtin_popcountll(left[i] & right[i]);
        }
        total_popcount = current_iter_popcount;
    }
    
    return total_popcount;
}