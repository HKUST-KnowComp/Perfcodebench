#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0 || probe_ts.empty() || dim_ts.empty()) {
        return 0;
    }

    const uint32_t* __restrict dts = dim_ts.data();
    const uint32_t* __restrict dvals = dim_values.data();
    const uint32_t* __restrict pts = probe_ts.data();
    const uint32_t* __restrict pw = probe_weights.data();

    const std::size_t n_probes = probe_ts.size();
    const std::size_t n_dims = dim_ts.size();

    uint64_t sum = 0;
    std::size_t dim_idx = 0;

    for (std::size_t i = 0; i < n_probes; ++i) {
        const uint32_t t = pts[i];
        while (dim_idx + 1 < n_dims && dts[dim_idx + 1] <= t) {
            ++dim_idx;
        }
        sum += static_cast<uint64_t>(dvals[dim_idx]) * static_cast<uint64_t>(pw[i]);
    }

    return sum;
}
