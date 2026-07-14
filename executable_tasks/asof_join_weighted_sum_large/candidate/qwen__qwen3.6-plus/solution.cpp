#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const uint32_t* dim_ts_ptr = dim_ts.data();
    const uint32_t* dim_val_ptr = dim_values.data();
    const uint32_t* probe_ts_ptr = probe_ts.data();
    const uint32_t* probe_w_ptr = probe_weights.data();
    
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        size_t d = 0;
        
        for (size_t p = 0; p < probe_size; ++p) {
            uint32_t pts = probe_ts_ptr[p];
            while (d + 1 < dim_size && dim_ts_ptr[d + 1] <= pts) {
                ++d;
            }
            iter_sum += static_cast<uint64_t>(dim_val_ptr[d]) * probe_w_ptr[p];
        }
        sum = iter_sum;
    }
    
    return sum;
}