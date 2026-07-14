#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t sum = 0;
    std::size_t d_idx = 0;
    const std::size_t dim_size = dim_ts.size();
    const std::size_t probe_size = probe_ts.size();
    for (std::size_t p = 0; p < probe_size; ++p) {
        // advance dim index while next dimension timestamp is <= current probe timestamp
        while (d_idx + 1 < dim_size && dim_ts[d_idx + 1] <= probe_ts[p]) {
            ++d_idx;
        }
        // dim_ts[d_idx] is the latest not-greater dimension timestamp
        sum += static_cast<uint64_t>(dim_values[d_idx]) * static_cast<uint64_t>(probe_weights[p]);
    }
    return sum;
}
