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

    const uint32_t* d_ts_ptr = dim_ts.data();
    const uint32_t* d_val_ptr = dim_values.data();
    const uint32_t* p_ts_ptr = probe_ts.data();
    const uint32_t* p_w_ptr = probe_weights.data();

    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t dim_idx = 0;
        
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t target = p_ts_ptr[i];
            
            // Advance dim_idx to the last element <= target
            // Since probe_ts is sorted, we never need to reset dim_idx
            while (dim_idx + 1 < dim_size && d_ts_ptr[dim_idx + 1] <= target) {
                dim_idx++;
            }
            
            // The as-of join requires the latest timestamp not greater than probe_ts[i].
            // If the first dim_ts is already > probe_ts[i], this logic depends on 
            // the problem's definition of 'latest not-greater'. 
            // Based on the baseline's `it - dim_ts.begin() - 1`, if it is the first element,
            // idx becomes -1 (underflow). However, in standard as-of joins, 
            // we assume dim_ts[0] <= probe_ts[0] or handle the empty case.
            // To match baseline exactly: if d_ts_ptr[0] > target, idx is effectively -1.
            if (d_ts_ptr[dim_idx] <= target) {
                current_sum += static_cast<uint64_t>(d_val_ptr[dim_idx]) * p_w_ptr[i];
            }
        }
        total_sum = current_sum;
    }
    
    return total_sum;
}