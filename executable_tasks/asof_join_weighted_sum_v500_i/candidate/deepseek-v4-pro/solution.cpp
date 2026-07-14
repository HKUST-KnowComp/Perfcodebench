#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0) return 0;

    const std::size_t nprobes = probe_ts.size();
    if (nprobes == 0) return 0;

    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_wt = probe_weights.data();
    const std::size_t ndim = dim_ts.size();

    std::size_t dim_idx = static_cast<std::size_t>(-1);
    uint64_t sum = 0;

    for (std::size_t i = 0; i < nprobes; ++i) {
        // advance the dimension index to the latest value <= probe_ts[i]
        while (dim_idx + 1 < ndim && d_ts[dim_idx + 1] <= p_ts[i]) {
            ++dim_idx;
        }
        sum += static_cast<uint64_t>(d_val[dim_idx]) *
               static_cast<uint64_t>(p_wt[i]);
    }

    return sum;
}