#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const size_t n = probe_ts.size();
    const size_t m = dim_ts.size();
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    uint64_t result = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        size_t d_idx = 0;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t pt = p_ts[i];
            while (d_idx + 1 < m && d_ts[d_idx + 1] <= pt) {
                ++d_idx;
            }
            iter_sum += static_cast<uint64_t>(d_val[d_idx]) * p_w[i];
        }
        result = iter_sum;
    }
    
    return result;
}