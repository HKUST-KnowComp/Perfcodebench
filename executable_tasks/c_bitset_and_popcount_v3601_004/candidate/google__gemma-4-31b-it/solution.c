#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total_popcount = 0;
    
    // The original code reset 'total' to 0 inside the loop, 
    // meaning only the result of the last iteration mattered.
    // However, the baseline logic was: 
    // for (iter < iters) { ... total = 0; for (...) total += bits[i]; }
    // This implies the function returns the popcount of the AND of the two sets 
    // for the final iteration of the loop.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < word_count; ++i) {
            current_iter_sum += __builtin_popcountll(left[i] & right[i]);
        }
        total_popcount = current_iter_sum;
    }
    
    return total_popcount;
}