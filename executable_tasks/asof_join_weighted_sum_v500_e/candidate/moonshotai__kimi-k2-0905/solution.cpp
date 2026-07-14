#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const std::size_t n_dim   = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;
        for (std::size_t p = 0; p < n_probe; ++p) {
            const uint32_t ts_p = probe_ts[p];
            while (dim_idx + 1 < n_dim && dim_ts[dim_idx + 1] <= ts_p)
                ++dim_idx;
            sum += static_cast<uint64_t>(dim_values[dim_idx]) *
                   static_cast<uint64_t>(probe_weights[p]);
        }
    }
    return sum;
}