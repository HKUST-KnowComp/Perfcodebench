#include "interface.h"
#include <cstdint>
#include <vector>
#include <limits>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build a direct key→value lookup table once.
    // Dimension is dense, so keys are 0…dim_keys.size()-1.
    std::vector<uint32_t> map(dim_keys.size());
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        map[dim_keys[i]] = dim_values[i];
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint32_t v = map[k];          // branch-free lookup
            sum += static_cast<uint64_t>(v) * static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}