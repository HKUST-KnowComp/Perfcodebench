#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    size_t n = probe_ts.size();
    size_t m = dim_ts.size();
    if (n == 0 || m == 0) return 0;

    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t dim_idx = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t p = p_ts[i];
            while (dim_idx + 1 < m && d_ts[dim_idx + 1] <= p) {
                ++dim_idx;
            }
            sum += (uint64_t)d_val[dim_idx] * p_w[i];
        }
    }
    return sum;
}