#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized range bucketing for sorted inputs.
 * Since both 'boundaries' and 'probes' are sorted, we can use a two-pointer
 * approach (similar to the merge step in merge sort) to find the upper bound
 * of each probe in O(N + M) time instead of O(M log N).
 */
uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (iters <= 0) return 0;

    const size_t n = boundaries.size();
    const size_t m = probes.size();
    const uint32_t* __restrict b_ptr = boundaries.data();
    const uint32_t* __restrict p_ptr = probes.data();

    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t b_idx = 0;

        // Two-pointer linear scan
        for (size_t p_idx = 0; p_idx < m; ++p_idx) {
            uint32_t p_val = p_ptr[p_idx];
            
            // Advance b_idx until boundaries[b_idx] > p_val
            // This is equivalent to std::upper_bound index
            while (b_idx < n && b_ptr[b_idx] <= p_val) {
                b_idx++;
            }
            
            current_sum += static_cast<uint64_t>(b_idx);
        }
        last_sum = current_sum;
    }

    return last_sum;
}
