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
    const uint32_t * const __restrict dts  = dim_ts.data();
    const uint32_t * const __restrict dval = dim_values.data();
    const uint32_t * const __restrict pts  = probe_ts.data();
    const uint32_t * const __restrict pw   = probe_weights.data();

    const std::size_t dim_n  = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;
        uint32_t  cur_val   = dval[0];

        for (std::size_t p = 0; p < probe_n; ++p) {
            const uint32_t pt = pts[p];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= pt) {
                ++dim_idx;
                cur_val = dval[dim_idx];
            }
            sum += static_cast<uint64_t>(cur_val) * static_cast<uint64_t>(pw[p]);
        }
    }
    return sum;
}