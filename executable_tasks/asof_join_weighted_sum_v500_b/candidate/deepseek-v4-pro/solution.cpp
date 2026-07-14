#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
    uint64_t sum = 0;
    std::size_t dim_idx = 0;
    const std::size_t dim_n = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();
    for (std::size_t i = 0; i < probe_n; ++i) {
        uint32_t probe_t = probe_ts[i];
        while (dim_idx + 1 < dim_n && dim_ts[dim_idx + 1] <= probe_t) {
            ++dim_idx;
        }
        sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
}
