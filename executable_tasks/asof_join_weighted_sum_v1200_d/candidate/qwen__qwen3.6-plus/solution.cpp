#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const std::size_t dim_size = dim_ts.size();
    const std::size_t probe_size = probe_ts.size();
    
    if (dim_size == 0 || probe_size == 0) return 0;
    
    const uint32_t* dim_ts_ptr = dim_ts.data();
    const uint32_t* dim_val_ptr = dim_values.data();
    const uint32_t* probe_ts_ptr = probe_ts.data();
    const uint32_t* probe_w_ptr = probe_weights.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        std::size_t dim_idx = 0;
        
        for (std::size_t i = 0; i < probe_size; ++i) {
            uint32_t pt = probe_ts_ptr[i];
            while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= pt) {
                ++dim_idx;
            }
            iter_sum += static_cast<uint64_t>(dim_val_ptr[dim_idx]) * probe_w_ptr[i];
        }
        result = iter_sum;
    }
    
    return result;
}