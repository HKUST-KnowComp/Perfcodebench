#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t sum = 0;
    size_t dim_size = dim_ts.size();
    size_t probe_size = probe_ts.size();
    size_t j = 0;
    for (size_t i = 0; i < probe_size; ++i) {
        while (j < dim_size && dim_ts[j] <= probe_ts[i]) {
            ++j;
        }
        // j points to first dim_ts > probe_ts[i]; j-1 is last dim_ts <= probe_ts[i]
        size_t idx = j - 1;
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum * static_cast<uint64_t>(iters);
}
