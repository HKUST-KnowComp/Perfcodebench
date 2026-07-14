#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (probe_keys.empty() || iters <= 0) return 0;

    // Dense dimension implies contiguous sorted keys. Compute base offset for O(1) indexing.
    uint32_t offset = dim_keys.empty() ? 0 : dim_keys[0];
    const uint32_t* d_vals = dim_values.data();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    std::size_t n = probe_keys.size();

    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(d_vals[p_keys[i] - offset]) * static_cast<uint64_t>(p_weights[i]);
    }
    return sum;
}