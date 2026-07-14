#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters <= 0 || probe_ts.empty() || dim_ts.empty()) {
        return 0;
    }

    const std::size_t probe_count = probe_ts.size();
    const std::size_t dim_count = dim_ts.size();

    uint64_t sum = 0;
    std::size_t d = 0;

    for (std::size_t p = 0; p < probe_count; ++p) {
        const uint32_t target = probe_ts[p];
        while (d + 1 < dim_count && dim_ts[d + 1] <= target) {
            ++d;
        }
        sum += static_cast<uint64_t>(dim_values[d]) * static_cast<uint64_t>(probe_weights[p]);
    }

    return sum;
}
