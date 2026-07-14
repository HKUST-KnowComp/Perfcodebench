#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized as-of join weighted sum.
 * 
 * The problem specifies that probe timestamps are sorted. 
 * Since both dim_ts and probe_ts are sorted, we can use a two-pointer approach 
 * to find the 'latest not-greater' dimension timestamp for each probe timestamp.
 * This reduces the complexity from O(iters * M * log N) to O(iters * (N + M)).
 */
uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (probe_ts.empty()) return 0;
    
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t dim_idx = 0;

        for (size_t i = 0; i < probe_size; ++i) {
            const uint32_t current_probe_ts = p_ts[i];
            
            // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= current_probe_ts
            // We check the next element to see if it still satisfies the condition.
            while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= current_probe_ts) {
                dim_idx++;
            }
            
            // The baseline implementation assumes dim_ts[0] <= probe_ts[i] or handles 
            // the underflow of (it - begin - 1) implicitly. 
            // We follow the logic: sum += dim_values[dim_idx] * probe_weights[i]
            current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
        }
        total_sum = current_sum;
    }

    return total_sum;
}
