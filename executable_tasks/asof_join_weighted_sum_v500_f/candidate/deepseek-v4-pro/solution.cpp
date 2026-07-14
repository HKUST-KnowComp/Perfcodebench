#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0) return 0;
    const std::size_t num_probes = probe_ts.size();
    const std::size_t num_dims = dim_ts.size();
    uint64_t sum = 0;
    std::size_t dim_idx = 0;
    for (std::size_t i = 0; i < num_probes; ++i) {
        while (dim_idx < num_dims && dim_ts[dim_idx] <= probe_ts[i]) {
            ++dim_idx;
        }
        // dim_idx points to first element > probe_ts[i] (or end)
        // so the latest not-greater is at dim_idx - 1
        sum += static_cast<uint64_t>(dim_values[dim_idx - 1]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
}