#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    std::vector<uint64_t> contributions(probe_ts.size());
    size_t dim_idx = 0;
    size_t dim_len = dim_ts.size();
    for (size_t i = 0; i < probe_ts.size(); ++i) {
        while (dim_idx < dim_len && dim_ts[dim_idx] <= probe_ts[i]) {
            ++dim_idx;
        }
        contributions[i] = static_cast<uint64_t>(dim_values[dim_idx - 1]) *
                           static_cast<uint64_t>(probe_weights[i]);
    }
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < contributions.size(); ++i) {
            sum += contributions[i];
        }
    }
    return sum;
}
