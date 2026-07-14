#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // Dense dimension: keys are sorted and have a small contiguous range.
    // Build a direct lookup array covering [min_key, max_key].
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    size_t range = static_cast<size_t>(max_key - min_key) + 1;
    std::vector<uint32_t> lookup(range, 0);

    // Populate lookup table: key -> dim_values[i]
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        // Optimized probe loop: single subtract and array index
        for (size_t i = 0; i < probe_keys.size(); ++i) {
            uint32_t offset = probe_keys[i] - min_key;
            sum += static_cast<uint64_t>(lookup[offset]) * static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}
