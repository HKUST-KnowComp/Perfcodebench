#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized as-of join weighted sum.
 * 
 * The problem specifies 'sorted probe timestamps', which allows for a two-pointer 
 * approach (linear scan) instead of binary search for each probe. 
 * This reduces complexity from O(Iters * P * log D) to O(Iters * (P + D)).
 */
uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const size_t num_dims = dim_ts.size();
    const size_t num_probes = probe_ts.size();

    if (num_dims == 0 || num_probes == 0) {
        return 0;
    }

    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t dim_idx = 0;

        // Linear scan utilizing the sorted property of both arrays
        for (size_t i = 0; i < num_probes; ++i) {
            const uint32_t target = p_ts[i];
            
            // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= target
            // We use a small optimization to skip multiple dim elements if possible
            while (dim_idx + 1 < num_dims && d_ts[dim_idx + 1] <= target) {
                dim_idx++;
            }
            
            current_iter_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}
