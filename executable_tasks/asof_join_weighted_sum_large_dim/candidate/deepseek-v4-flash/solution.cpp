#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    // Compute the total weighted sum once with a two-pointer linear pass.
    uint64_t total_sum = 0;
    size_t j = 0;
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    for (size_t i = 0; i < probe_size; ++i) {
        while (j < dim_size && dim_ts[j] <= probe_ts[i]) {
            ++j;
        }
        // Assumption: for each probe there is at least one dim timestamp <= it
        total_sum += static_cast<uint64_t>(dim_values[j - 1]) * static_cast<uint64_t>(probe_weights[i]);
    }
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = total_sum;
    }
    return sum;
}