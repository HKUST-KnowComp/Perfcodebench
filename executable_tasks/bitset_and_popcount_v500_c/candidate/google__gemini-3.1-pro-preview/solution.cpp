#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized bitset intersection popcount.
 * 
 * The original implementation was extremely slow because it expanded each bit into a 
 * std::vector<uint8_t> and then summed them. This version uses the hardware-accelerated
 * __builtin_popcountll (which maps to the POPCNT instruction on modern x86_64 CPUs).
 * 
 * We also apply loop unrolling to maximize throughput and minimize branch overhead.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    
    const std::size_t n = left.size();
    uint64_t final_count = 0;

    // The benchmark repeats the operation 'iters' times.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_count = 0;
        std::size_t i = 0;

        // Unroll the loop by 4 to improve instruction-level parallelism.
        // Modern CPUs can often execute multiple POPCNT instructions per cycle.
        for (; i + 3 < n; i += 4) {
            uint64_t w0 = left[i] & right[i];
            uint64_t w1 = left[i + 1] & right[i + 1];
            uint64_t w2 = left[i + 2] & right[i + 2];
            uint64_t w3 = left[i + 3] & right[i + 3];

            current_count += static_cast<uint64_t>(__builtin_popcountll(w0));
            current_count += static_cast<uint64_t>(__builtin_popcountll(w1));
            current_count += static_cast<uint64_t>(__builtin_popcountll(w2));
            current_count += static_cast<uint64_t>(__builtin_popcountll(w3));
        }

        // Handle remaining elements.
        for (; i < n; ++i) {
            current_count += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
        }

        final_count = current_count;
    }

    return final_count;
}