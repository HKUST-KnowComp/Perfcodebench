#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    // Pre-calculate the popcount for a single pass to avoid redundant work if iters > 0
    // However, the task implies we must simulate the 'iters' loop behavior.
    // The baseline code resets 'count' every iteration, so the final return is the count of the LAST iteration.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_count = 0;
        for (uint64_t word : words) {
            current_iter_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        total_count = current_iter_count;
    }
    
    // If iters is 0, the baseline would return 0 (count initialized to 0).
    // If iters > 0, it returns the count of the last iteration.
    return total_count;
}