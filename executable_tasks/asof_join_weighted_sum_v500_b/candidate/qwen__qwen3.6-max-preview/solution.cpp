#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const std::size_t n_dim = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    uint64_t sum = 0;
    const std::size_t max_dim_idx = n_dim > 0 ? n_dim - 1 : 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        std::size_t dim_idx = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            uint32_t pt = p_ts[i];
            while (dim_idx < max_dim_idx && d_ts[dim_idx + 1] <= pt) {
                ++dim_idx;
            }
            current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
        }
        sum = current_sum;
    }
    return sum;
}