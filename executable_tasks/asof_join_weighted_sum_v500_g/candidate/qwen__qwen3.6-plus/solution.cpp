#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (iters <= 0 || probe_ts.empty() || dim_ts.empty()) return 0;
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    const size_t dim_n = dim_ts.size();
    const size_t probe_n = probe_ts.size();
    
    uint64_t last_sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t d_idx = 0;
        for (size_t i = 0; i < probe_n; ++i) {
            uint32_t t = p_ts[i];
            while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= t) {
                ++d_idx;
            }
            sum += static_cast<uint64_t>(d_val[d_idx]) * p_w[i];
        }
        last_sum = sum;
    }
    return last_sum;
}