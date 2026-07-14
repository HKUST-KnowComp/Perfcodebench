#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    (void)iters;
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    
    size_t d_size = dim_ts.size();
    size_t p_size = probe_ts.size();
    
    if (d_size == 0 || p_size == 0) return 0;
    
    uint64_t sum = 0;
    size_t d = 0;
    size_t d_limit = d_size - 1;
    
    for (size_t i = 0; i < p_size; ++i) {
        uint32_t cur_ts = p_ts[i];
        while (d < d_limit && d_ts[d + 1] <= cur_ts) {
            ++d;
        }
        sum += static_cast<uint64_t>(d_val[d]) * p_w[i];
    }
    
    return sum;
}