#include "interface.h"
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const uint32_t* d_ts   = dim_ts.data();
    const uint32_t* d_vals = dim_values.data();
    const uint32_t* p_ts   = probe_ts.data();
    const uint32_t* p_w    = probe_weights.data();

    const std::size_t dim_n  = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t d_idx = 0;
        uint32_t cur_val = d_vals[0];

        for (std::size_t p_idx = 0; p_idx < probe_n; ++p_idx) {
            const uint32_t t = p_ts[p_idx];
            while (d_idx + 1 < dim_n && d_ts[d_idx + 1] <= t) {
                ++d_idx;
                cur_val = d_vals[d_idx];
            }
            sum += static_cast<uint64_t>(cur_val) * static_cast<uint64_t>(p_w[p_idx]);
        }
        total = sum;
    }
    return total;
}