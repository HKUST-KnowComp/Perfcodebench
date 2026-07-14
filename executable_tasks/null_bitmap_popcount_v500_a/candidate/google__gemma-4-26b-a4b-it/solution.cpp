#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    // Pre-calculate the popcount of each word once to avoid redundant work if iters > 1
    // However, the task implies we must simulate the loop structure. 
    // To maximize performance while respecting the 'iters' loop, we compute the sum.
    
    // Optimization: The baseline was doing a massive amount of work inside the loop.
    // We can compute the popcount of the entire vector once and multiply by iters,
    // or simply run the loop as requested but with the efficient intrinsic.
    
    // To ensure we match the behavior of the baseline (which returns the count of the LAST iteration),
    // we follow the logic: count = sum of bits in words.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_count = 0;
        for (uint64_t word : words) {
            current_iter_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        total_count = current_iter_count;
    }
    
    return total_count;
}