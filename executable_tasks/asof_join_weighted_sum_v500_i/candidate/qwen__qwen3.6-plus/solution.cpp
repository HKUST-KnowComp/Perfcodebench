#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    (void)iters;
    const size_t d_size = dim_ts.size();
    const size_t p_size = probe_ts.size();
    if (d_size == 0 || p_size == 0) return 0;
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_wt = probe_weights.data();
    
    uint64_t sum = 0;
    size_t d_idx = 0;
    
    for (size_t i = 0; i < p_size; ++i) {
        uint32_t pt = p_ts[i];
        while (d_idx + 1 < d_size && d_ts[d_idx + 1] <= pt) {
            ++d_idx;
        }
        sum += static_cast<uint64_t>(d_val[d_idx]) * p_wt[i];
    }
    return sum;
}