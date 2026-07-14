#include "interface.h"
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    uint64_t sum = 0;
    const size_t dim_limit = dim_size > 0 ? dim_size - 1 : 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t dim_idx = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t pt = p_ts[i];
            while (dim_idx < dim_limit && d_ts[dim_idx + 1] <= pt) {
                ++dim_idx;
            }
            sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
        }
    }
    return sum;
}