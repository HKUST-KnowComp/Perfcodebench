#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const std::size_t nprobe = probe_ts.size();
    if (nprobe == 0) return 0;
    std::vector<std::size_t> indices(nprobe);
    std::size_t di = 0;
    const std::size_t ndim = dim_ts.size();
    for (std::size_t i = 0; i < nprobe; ++i) {
        const uint32_t target = probe_ts[i];
        while (di < ndim && dim_ts[di] <= target) ++di;
        // di now points to first element > target, so last not-greater is at di-1
        indices[i] = di - 1; // assuming probe_ts[i] >= dim_ts[0]
    }
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < nprobe; ++i) {
            local_sum += static_cast<uint64_t>(dim_values[indices[i]]) * static_cast<uint64_t>(probe_weights[i]);
        }
        sum = local_sum;
    }
    return sum;
}