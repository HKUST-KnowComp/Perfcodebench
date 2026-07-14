#include "interface.h"

#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build direct lookup table once per call.
    // Dimension keys are dense 0..max_key, so we can index directly.
    uint32_t max_key = 0;
    if (!dim_keys.empty()) {
        max_key = *std::max_element(dim_keys.begin(), dim_keys.end());
    }
    std::vector<uint32_t> value_of(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        value_of[dim_keys[i]] = dim_values[i];
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t k = probe_keys[i];
            uint32_t w = probe_weights[i];
            sum += static_cast<uint64_t>(value_of[k]) * static_cast<uint64_t>(w);
        }
    }
    return sum;
}