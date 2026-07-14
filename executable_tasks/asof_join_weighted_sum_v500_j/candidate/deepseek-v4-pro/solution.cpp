#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0 || probe_ts.empty()) return 0;

    // Precompute the matched dimension value for each probe timestamp
    std::vector<uint32_t> matched_values(probe_ts.size());
    size_t j = 0;
    const size_t n_dim = dim_ts.size();
    for (size_t i = 0; i < probe_ts.size(); ++i) {
        while (j < n_dim && dim_ts[j] <= probe_ts[i]) {
            ++j;
        }
        // j now points to the first dim_ts > probe_ts[i], so match is at j-1
        matched_values[i] = dim_values[j - 1];
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (size_t i = 0; i < probe_ts.size(); ++i) {
            local_sum += static_cast<uint64_t>(matched_values[i]) * static_cast<uint64_t>(probe_weights[i]);
        }
        sum = local_sum;
    }
    return sum;
}
