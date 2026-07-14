#include "interface.h"
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_ts.empty() || probe_ts.empty()) return 0;
    uint64_t sum_one = 0;
    size_t j = 0;
    const size_t n = dim_ts.size();
    for (size_t i = 0; i < probe_ts.size(); ++i) {
        while (j < n && dim_ts[j] <= probe_ts[i]) {
            ++j;
        }
        sum_one += static_cast<uint64_t>(dim_values[j - 1]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum_one * static_cast<uint64_t>(iters);
}