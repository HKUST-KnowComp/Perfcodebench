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

    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t dim_idx = 0;
        
        for (size_t probe_idx = 0; probe_idx < probe_size; ++probe_idx) {
            uint32_t target = p_ts[probe_idx];
            
            // Advance dim_idx to the last element <= target
            // Since probe_ts is sorted, we don't need to reset dim_idx
            while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= target) {
                dim_idx++;
            }
            
            // The as-of join logic: find the latest timestamp <= probe_ts[i]
            // If the first dim_ts is already > probe_ts[0], this logic needs to handle it.
            // However, the baseline used upper_bound - 1, which implies dim_ts[0] is the default
            // if probe_ts[i] < dim_ts[0] (though that would result in index -1, which is UB).
            // Assuming dim_ts[0] <= probe_ts[0] based on typical as-of join constraints or 
            // that the baseline's behavior on probe_ts[i] < dim_ts[0] is acceptable.
            
            current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[probe_idx]);
        }
        total_sum = current_sum;
    }
    return total_sum;
}