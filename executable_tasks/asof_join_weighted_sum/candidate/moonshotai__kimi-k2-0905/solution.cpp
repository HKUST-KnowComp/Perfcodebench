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
    const uint32_t*  d_ts   = dim_ts.data();
    const uint32_t*  d_val  = dim_values.data();
    const uint32_t*  p_ts   = probe_ts.data();
    const uint32_t*  p_w    = probe_weights.data();

    const size_t dim_n = dim_ts.size();
    const size_t probe_n = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t d = 0;
        for (size_t p = 0; p < probe_n; ++p) {
            const uint32_t target = p_ts[p];
            while (d + 1 < dim_n && d_ts[d + 1] <= target) {
                ++d;
            }
            sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_w[p]);
        }
    }
    return sum;
}