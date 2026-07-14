#include "interface.h"
#include <cstdint>
#include <vector>

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
    const std::size_t n_dim = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            uint32_t pt = p_ts[i];
            while (dim_idx + 1 < n_dim && d_ts[dim_idx + 1] <= pt) {
                ++dim_idx;
            }
            sum += static_cast<uint64_t>(d_val[dim_idx]) * p_w[i];
        }
    }
    return sum;
}
