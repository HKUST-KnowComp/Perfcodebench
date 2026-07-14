#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    // Pre-calculate the popcount for each word once to avoid redundant work if iters > 1
    // However, the task implies we must simulate the loop structure. 
    // To maximize performance, we process the words directly.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_count = 0;
        for (uint64_t word : words) {
            iter_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        total_count = iter_count;
    }
    
    return total_count;
}