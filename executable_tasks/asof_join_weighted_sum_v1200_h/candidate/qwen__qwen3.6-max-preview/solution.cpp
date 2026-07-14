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
    
    uint64_t sum = 0;
    const std::size_t n_dim = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        std::size_t j = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            uint32_t pt = p_ts[i];
            while (j + 1 < n_dim && d_ts[j + 1] <= pt) {
                ++j;
            }
            current_sum += static_cast<uint64_t>(d_val[j]) * p_w[i];
        }
        sum = current_sum;
    }
    return sum;
}
