#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0) {
        return 0;
    }
    const uint32_t* ts = dim_ts.data();
    const uint32_t* vals = dim_values.data();
    const uint32_t* probes = probe_ts.data();
    const uint32_t* weights = probe_weights.data();
    const size_t n = dim_ts.size();
    const size_t m = probe_ts.size();

    uint64_t sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < m; ++i) {
        uint32_t probe = probes[i];
        while (dim_idx + 1 < n && ts[dim_idx + 1] <= probe) {
            ++dim_idx;
        }
        // dim_idx points to largest ts[dim_idx] <= probe
        // (assumes probe >= ts[0]; otherwise original code had UB)
        sum += static_cast<uint64_t>(vals[dim_idx]) * static_cast<uint64_t>(weights[i]);
    }

    return sum;
}
