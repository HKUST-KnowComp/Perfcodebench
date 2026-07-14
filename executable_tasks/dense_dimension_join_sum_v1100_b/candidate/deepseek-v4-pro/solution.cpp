#include "interface.h"

#include <cstdint>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty()) return 0;
    const uint32_t base = dim_keys[0];
    const std::size_t N = dim_keys.size();
    uint64_t sum = 0;
    const std::size_t probe_count = probe_keys.size();
    for (std::size_t i = 0; i < probe_count; ++i) {
        int64_t offset = static_cast<int64_t>(probe_keys[i]) - base;
        std::size_t idx;
        if (offset < 0) {
            idx = 0;
        } else {
            std::size_t uoffset = static_cast<std::size_t>(offset);
            idx = (uoffset >= N) ? N : uoffset;
        }
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
}
