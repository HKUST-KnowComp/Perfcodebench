#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const size_t n_dim = dim_ts.size();
    const size_t n_probe = probe_ts.size();
    const uint32_t* dts = dim_ts.data();
    const uint32_t* dvals = dim_values.data();
    const uint32_t* pts = probe_ts.data();
    const uint32_t* pweights = probe_weights.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        int64_t j = -1; // index of the last dim_ts <= current probe
        for (size_t i = 0; i < n_probe; ++i) {
            uint32_t p = pts[i];
            // Advance j to the largest index with dts[j] <= p
            while (static_cast<size_t>(j + 1) < n_dim && dts[j + 1] <= p) {
                ++j;
            }
            // j is guaranteed to be >= 0 for valid inputs (as in the baseline)
            sum += static_cast<uint64_t>(dvals[j]) * static_cast<uint64_t>(pweights[i]);
        }
    }
    return sum;
}