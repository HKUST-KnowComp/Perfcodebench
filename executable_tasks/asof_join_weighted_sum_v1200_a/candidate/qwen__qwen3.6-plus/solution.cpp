#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    const size_t n = probe_ts.size();
    const size_t m = dim_ts.size();
    
    uint64_t single_sum = 0;
    size_t di = 0;
    
    for (size_t i = 0; i < n; ++i) {
        uint32_t pt = p_ts[i];
        while (di + 1 < m && d_ts[di + 1] <= pt) {
            ++di;
        }
        single_sum += static_cast<uint64_t>(d_val[di]) * p_w[i];
    }
    
    return single_sum * iters;
}