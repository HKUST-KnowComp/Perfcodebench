#include "interface.h"

#include <cstddef>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    std::size_t dim_n = dim_ts.size();
    std::size_t probe_n = probe_ts.size();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

        if (dim_n == 0) {
            // No dimension timestamps -> no match possible
            continue;
        }

        std::size_t dim_idx = 0;

        for (std::size_t i = 0; i < probe_n; ++i) {
            uint32_t pt = probe_ts[i];

            // Advance dim_idx while the next dim timestamp is still <= probe
            while (dim_idx + 1 < dim_n && dim_ts[dim_idx + 1] <= pt) {
                ++dim_idx;
            }

            // If current dim timestamp is <= probe, use it; otherwise skip (add 0)
            if (dim_ts[dim_idx] <= pt) {
                sum += static_cast<uint64_t>(dim_values[dim_idx]) *
                       static_cast<uint64_t>(probe_weights[i]);
            }
        }
    }

    return sum;
}
