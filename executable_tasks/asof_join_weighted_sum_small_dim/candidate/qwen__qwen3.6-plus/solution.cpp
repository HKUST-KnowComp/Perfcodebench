#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    uint64_t sum = 0;
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    
    const uint32_t* dim_ts_ptr = dim_ts.data();
    const uint32_t* dim_values_ptr = dim_values.data();
    const uint32_t* probe_ts_ptr = probe_ts.data();
    const uint32_t* probe_weights_ptr = probe_weights.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t dim_idx = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t pt = probe_ts_ptr[i];
            while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= pt) {
                ++dim_idx;
            }
            sum += static_cast<uint64_t>(dim_values_ptr[dim_idx]) * probe_weights_ptr[i];
        }
    }
    return sum;
}