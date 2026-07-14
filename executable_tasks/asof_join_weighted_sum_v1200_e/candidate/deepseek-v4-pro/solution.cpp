#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    (void)iters; // result does not depend on number of iterations
    const size_t n = probe_ts.size();
    const size_t m = dim_ts.size();
    uint64_t sum = 0;
    size_t d = 0;
    for (size_t i = 0; i < n; ++i) {
        while (d + 1 < m && dim_ts[d + 1] <= probe_ts[i]) {
            ++d;
        }
        sum += static_cast<uint64_t>(dim_values[d]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
}
