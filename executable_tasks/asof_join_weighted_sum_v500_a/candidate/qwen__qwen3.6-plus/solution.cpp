#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    size_t n = probe_ts.size();
    size_t m = dim_ts.size();
    
    if (n == 0 || m == 0) return 0;
    
    std::vector<uint32_t> matched_values(n);
    const uint32_t* dts = dim_ts.data();
    const uint32_t* dvals = dim_values.data();
    const uint32_t* pts = probe_ts.data();
    
    size_t j = 0;
    for (size_t i = 0; i < n; ++i) {
        uint32_t pt = pts[i];
        while (j + 1 < m && dts[j + 1] <= pt) {
            ++j;
        }
        matched_values[i] = dvals[j];
    }
    
    const uint32_t* pweights = probe_weights.data();
    const uint32_t* mvals = matched_values.data();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            iter_sum += static_cast<uint64_t>(mvals[i]) * static_cast<uint64_t>(pweights[i]);
        }
        sum = iter_sum;
    }
    return sum;
}