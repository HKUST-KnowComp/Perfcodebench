#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build direct lookup table: lut[key] = value
    std::vector<uint32_t> lut;
    if (!dim_keys.empty()) {
        const uint32_t max_key = dim_keys.back();
        lut.resize(max_key + 1);
        for (size_t i = 0; i < dim_keys.size(); ++i)
            lut[dim_keys[i]] = dim_values[i];
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            sum += static_cast<uint64_t>(lut[k]) * static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}