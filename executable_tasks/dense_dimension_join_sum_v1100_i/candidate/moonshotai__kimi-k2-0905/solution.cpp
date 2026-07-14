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
    // Build a direct lookup table once per call.
    // Dimension keys are dense 0..kMax, so value = lut[key].
    const uint32_t kMax = dim_keys.back();
    std::vector<uint32_t> lut(kMax + 1);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lut[dim_keys[i]] = dim_values[i];
    }

    const size_t n = probe_keys.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (size_t i = 0; i < n; ++i) {
            local += static_cast<uint64_t>(lut[probe_keys[i]]) *
                     static_cast<uint64_t>(probe_weights[i]);
        }
        sum = local;
    }
    return sum;
}