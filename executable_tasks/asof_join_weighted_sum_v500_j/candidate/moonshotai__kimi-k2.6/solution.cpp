#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0) return 0;

    const std::size_t dim_n = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();
    if (dim_n == 0 || probe_n == 0) return 0;

    const uint32_t* dts = dim_ts.data();
    const uint32_t* dvals = dim_values.data();
    const uint32_t* pts = probe_ts.data();
    const uint32_t* pw = probe_weights.data();

    uint64_t sum = 0;
    std::size_t d = 0;

    for (std::size_t i = 0; i < probe_n; ++i) {
        const uint32_t t = pts[i];
        while (d + 1 < dim_n && dts[d + 1] <= t) {
            ++d;
        }
        if (dts[d] <= t) {
            sum += static_cast<uint64_t>(dvals[d]) * static_cast<uint64_t>(pw[i]);
        }
    }

    return sum;
}
