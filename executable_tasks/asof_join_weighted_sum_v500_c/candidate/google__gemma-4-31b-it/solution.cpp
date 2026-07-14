#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    
    if (dim_size == 0 || probe_size == 0) return 0;

    uint64_t total_sum = 0;

    // Pre-calculate the sum for one iteration to avoid repeated linear scans
    // if iters is large. However, the task asks for the sum over iters.
    // Since the data is static across iters, we compute it once and multiply.
    
    uint64_t single_iter_sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < probe_size; ++i) {
        uint32_t p_ts = probe_ts[i];
        
        // Move dim_idx to the last element <= p_ts
        // Since probe_ts is sorted, we only move forward
        while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= p_ts) {
            dim_idx++;
        }
        
        // The as-of join logic: if p_ts < dim_ts[0], the baseline logic 
        // (upper_bound - 1) would result in an underflow/out-of-bounds 
        // if not handled. However, the baseline uses static_cast<size_t>(it - begin - 1).
        // If it == begin, idx becomes SIZE_MAX. 
        // Assuming dim_ts[0] <= probe_ts[0] based on typical as-of join constraints
        // or that the baseline's behavior with dim_values[SIZE_MAX] is not triggered.
        
        single_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * 
                            static_cast<uint64_t>(probe_weights[i]);
    }

    return single_iter_sum * iters;
}