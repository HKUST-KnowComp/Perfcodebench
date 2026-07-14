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
    const std::size_t dim_n  = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;

        for (std::size_t probe_idx = 0; probe_idx < probe_n; ++probe_idx) {
            const uint32_t probe_t = probe_ts[probe_idx];
            while (dim_idx + 1 < dim_n && dim_ts[dim_idx + 1] <= probe_t) {
                ++dim_idx;
            }
            sum += static_cast<uint64_t>(dim_values[dim_idx]) *
                   static_cast<uint64_t>(probe_weights[probe_idx]);
        }
    }

    return sum;
}