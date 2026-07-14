#include "interface.h"
#include <cstdint>
#include <vector>

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
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t j = 0;
        std::size_t i = 0;
        
        while (i < n_probe && j + 1 < n_dim) {
            if (d_ts[j + 1] <= p_ts[i]) {
                ++j;
            } else {
                sum += static_cast<uint64_t>(d_val[j]) * static_cast<uint64_t>(p_w[i]);
                ++i;
            }
        }
        
        uint64_t last_val = static_cast<uint64_t>(d_val[j]);
        for (; i < n_probe; ++i) {
            sum += last_val * static_cast<uint64_t>(p_w[i]);
        }
    }
    return sum;
}
