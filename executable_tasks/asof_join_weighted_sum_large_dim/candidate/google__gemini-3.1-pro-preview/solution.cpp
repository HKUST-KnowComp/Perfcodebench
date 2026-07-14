#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

/**
 * Optimized as-of join weighted sum.
 * 
 * The baseline uses std::upper_bound for every probe_ts, which is O(M log N).
 * Since both dim_ts and probe_ts are typically sorted in as-of join scenarios,
 * we can use a two-pointer approach to achieve O(N + M) per iteration.
 * 
 * Given the task title "sorted as-of join [large dim]", we assume probe_ts is sorted.
 * If probe_ts were not sorted, we could sort it once (O(M log M)) and then use 
 * the two-pointer approach, which would still be faster than the baseline for large iters.
 */
uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (probe_ts.empty() || dim_ts.empty()) {
        return 0;
    }

    uint64_t final_sum = 0;
    const size_t probe_size = probe_ts.size();
    const size_t dim_size = dim_ts.size();

    // Cache pointers to raw data to avoid vector overhead inside hot loops
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t dim_idx = 0;

        for (size_t i = 0; i < probe_size; ++i) {
            const uint32_t target = p_ts[i];

            // Move the dimension index forward to find the latest timestamp 
            // that is not greater than the current probe timestamp.
            // This assumes probe_ts is sorted.
            while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= target) {
                dim_idx++;
            }

            // The baseline uses: idx = upper_bound - 1.
            // If target < d_ts[0], the baseline would result in idx = -1 (SIZE_MAX).
            // We maintain that behavior for exact correctness, though it implies 
            // the test data likely ensures target >= d_ts[0].
            
            // Check if target is smaller than the very first dimension timestamp
            if (target < d_ts[0]) {
                // This matches the baseline's behavior of using (size_t)-1
                const size_t underflow_idx = static_cast<size_t>(-1);
                current_sum += static_cast<uint64_t>(d_val[underflow_idx]) * static_cast<uint64_t>(p_w[i]);
            } else {
                current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}
