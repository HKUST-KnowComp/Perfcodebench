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
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t j = 0;
        const std::size_t dim_size = dim_ts.size();
        const std::size_t probe_size = probe_ts.size();
        for (std::size_t i = 0; i < probe_size; ++i) {
            const uint32_t pt = probe_ts[i];
            // Advance j while next dim_ts is <= pt and j+1 is within bounds
            while (j + 1 < dim_size && dim_ts[j + 1] <= pt) {
                ++j;
            }
            local_sum += static_cast<uint64_t>(dim_values[j]) * static_cast<uint64_t>(probe_weights[i]);
        }
        sum = local_sum;
    }
    return sum;
}
