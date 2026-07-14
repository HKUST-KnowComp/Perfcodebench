#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t sum = 0;
    const std::size_t dim_len = dim_ts.size();
    const std::size_t probe_len = probe_ts.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;
        for (std::size_t i = 0; i < probe_len; ++i) {
            while (dim_idx < dim_len && dim_ts[dim_idx] <= probe_ts[i]) {
                ++dim_idx;
            }
            if (dim_idx > 0) {
                sum += static_cast<uint64_t>(dim_values[dim_idx - 1]) * static_cast<uint64_t>(probe_weights[i]);
            }
        }
    }
    return sum;
}