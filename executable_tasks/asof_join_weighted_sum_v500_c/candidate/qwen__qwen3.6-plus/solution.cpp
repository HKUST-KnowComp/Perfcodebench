#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (iters <= 0 || probe_ts.empty() || dim_ts.empty()) {
        return 0;
    }

    const size_t dim_n = dim_ts.size();
    const size_t probe_n = probe_ts.size();
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    uint64_t sum = 0;
    size_t dim_idx = 0;
    
    for (size_t i = 0; i < probe_n; ++i) {
        uint32_t cur_probe = p_ts[i];
        while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= cur_probe) {
            ++dim_idx;
        }
        sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
    }
    
    return sum;
}