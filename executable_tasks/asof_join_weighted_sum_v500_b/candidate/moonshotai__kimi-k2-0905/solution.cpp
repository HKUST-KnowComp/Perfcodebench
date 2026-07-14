#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const uint32_t*  d_ts  = dim_ts.data();
    const uint32_t*  d_val = dim_values.data();
    const uint32_t*  p_ts  = probe_ts.data();
    const uint32_t*  p_w   = probe_weights.data();
    const size_t     dim_n = dim_ts.size();
    const size_t     probe_n = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        size_t   dim_idx   = 0;
        uint32_t cur_val   = d_val[0];

        for (size_t probe_idx = 0; probe_idx < probe_n; ++probe_idx) {
            const uint32_t ts   = p_ts[probe_idx];
            const uint32_t w    = p_w[probe_idx];

            while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= ts) {
                ++dim_idx;
                cur_val = d_val[dim_idx];
            }

            local_sum += static_cast<uint64_t>(cur_val) * static_cast<uint64_t>(w);
        }
        sum = local_sum;
    }
    return sum;
}