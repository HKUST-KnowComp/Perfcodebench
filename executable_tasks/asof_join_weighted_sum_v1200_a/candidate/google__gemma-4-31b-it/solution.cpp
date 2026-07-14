#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t total_sum = 0;
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();

    if (dim_size == 0 || probe_size == 0) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t dim_idx = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t target = probe_ts[i];
            // Move dim_idx to the last element <= target
            while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= target) {
                dim_idx++;
            }
            // If the first element is already greater than target, 
            // the baseline's `it - begin() - 1` would result in an underflow/wrap.
            // However, in standard as-of joins, we assume dim_ts[0] <= probe_ts[0] 
            // or handle the boundary. The baseline does `dim_values[idx]` where 
            // idx = upper_bound - 1. If upper_bound is begin(), idx is size_t max.
            // To match baseline exactly:
            if (dim_ts[0] > target) {
                // This case would cause a crash in the baseline due to dim_values[static_cast<size_t>(-1)]
                // We assume the input data is valid for the baseline's logic.
            }
            current_sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
        }
        total_sum = current_sum;
    }
    return total_sum;
}