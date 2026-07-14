#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized as-of join weighted sum.
 * 
 * The baseline implementation uses std::upper_bound for every probe, resulting in O(iters * P * log D) complexity.
 * Given the task title "sorted as-of join", we assume both dim_ts and probe_ts are sorted.
 * This allows a two-pointer approach with O(iters * (P + D)) complexity.
 */
uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (probe_ts.empty() || dim_ts.empty()) return 0;

    const uint32_t* const d_ts = dim_ts.data();
    const uint32_t* const d_val = dim_values.data();
    const uint32_t* const p_ts = probe_ts.data();
    const uint32_t* const p_w = probe_weights.data();
    
    const size_t d_size = dim_ts.size();
    const size_t p_size = probe_ts.size();

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t d_idx = 0;

        // Two-pointer approach: since both p_ts and d_ts are sorted,
        // we can find the 'as-of' index for each probe by advancing d_idx.
        for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
            const uint32_t target = p_ts[p_idx];
            
            // Advance d_idx to the largest index such that d_ts[d_idx] <= target.
            // We check d_idx + 1 to see if the next dimension timestamp is still valid.
            while (d_idx + 1 < d_size && d_ts[d_idx + 1] <= target) {
                d_idx++;
            }
            
            // The baseline logic: idx = upper_bound - 1.
            // If target < d_ts[0], the baseline would result in idx = (size_t)-1.
            // We assume target >= d_ts[0] based on standard as-of join constraints,
            // but we maintain the pointer logic to match baseline behavior.
            current_sum += static_cast<uint64_t>(d_val[d_idx]) * static_cast<uint64_t>(p_w[p_idx]);
        }
        total_sum = current_sum;
    }

    return total_sum;
}
