#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    // Precompute for each probe timestamp the index of the matching dimension entry
    const std::size_t n_dim = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();
    std::vector<std::size_t> match_idx(n_probe);
    
    std::size_t i_dim = 0;
    for (std::size_t i_probe = 0; i_probe < n_probe; ++i_probe) {
        // Advance dim pointer while next dim timestamp is <= probe timestamp
        while (i_dim + 1 < n_dim && dim_ts[i_dim + 1] <= probe_ts[i_probe]) {
            ++i_dim;
        }
        match_idx[i_probe] = i_dim;
    }
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            sum += static_cast<uint64_t>(dim_values[match_idx[i]]) * 
                   static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}
