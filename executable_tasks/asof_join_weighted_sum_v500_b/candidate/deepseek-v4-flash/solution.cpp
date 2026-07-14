#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const std::size_t n_probe = probe_ts.size();
    const std::size_t n_dim = dim_ts.size();

    // Precompute the join index for each probe timestamp using a two-pointer scan.
    // Both sequences are sorted non-decreasing.
    std::vector<std::size_t> dim_idx(n_probe);
    std::size_t j = 0;
    for (std::size_t i = 0; i < n_probe; ++i) {
        // Advance j while the next dimension timestamp is still <= probe_ts[i]
        while (j + 1 < n_dim && dim_ts[j + 1] <= probe_ts[i]) {
            ++j;
        }
        dim_idx[i] = j;  // j is the last index with dim_ts[j] <= probe_ts[i]
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            sum += static_cast<uint64_t>(dim_values[dim_idx[i]]) *
                   static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}