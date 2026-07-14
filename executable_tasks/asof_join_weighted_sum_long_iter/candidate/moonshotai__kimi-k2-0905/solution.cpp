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
    const uint32_t* dts  = dim_ts.data();
    const uint32_t* dval = dim_values.data();
    const uint32_t* pts  = probe_ts.data();
    const uint32_t* pw   = probe_weights.data();

    const size_t dim_n = dim_ts.size();
    const size_t probe_n = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t dim_idx = 0;
        for (size_t p = 0; p < probe_n; ++p) {
            const uint32_t t = pts[p];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t)
                ++dim_idx;
            sum += static_cast<uint64_t>(dval[dim_idx]) * static_cast<uint64_t>(pw[p]);
        }
    }
    return sum;
}