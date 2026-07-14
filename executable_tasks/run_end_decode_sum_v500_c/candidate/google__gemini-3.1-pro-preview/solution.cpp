#pragma once

#include "interface.h"
#include <cstdint>
#include <vector>

/**
 * Optimized run-end decode aggregation.
 * 
 * Instead of expanding the run-end encoded data into a full vector (which is slow and memory-intensive),
 * we calculate the sum directly using the run lengths. 
 * 
 * For each run i:
 *   Length = run_ends[i] - run_ends[i-1]
 *   Value  = values[i]
 *   Sum contribution = Length * Value
 */
uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
    
    if (iters <= 0 || run_ends.empty()) {
        return 0;
    }

    uint64_t final_sum = 0;
    const size_t n = run_ends.size();
    const uint32_t* __restrict__ re_ptr = run_ends.data();
    const uint32_t* __restrict__ val_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        uint32_t prev = 0;
        
        // Unroll or vectorize the summation loop
        // The compiler can easily vectorize this linear scan.
        for (size_t i = 0; i < n; ++i) {
            const uint32_t current_end = re_ptr[i];
            const uint32_t length = current_end - prev;
            current_sum += static_cast<uint64_t>(length) * val_ptr[i];
            prev = current_end;
        }
        final_sum = current_sum;
    }

    return final_sum;
}