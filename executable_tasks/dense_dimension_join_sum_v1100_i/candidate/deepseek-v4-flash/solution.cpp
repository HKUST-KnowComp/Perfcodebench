#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    // Assume dim_keys are sorted and dense (contiguous integers without gaps)
    if (dim_keys.empty())
        return 0;

    const uint32_t min_key = dim_keys.front();
    const uint32_t max_key = dim_keys.back();
    const std::size_t range = static_cast<std::size_t>(max_key - min_key + 1);

    // Build direct lookup table: key -> dim_value
    std::vector<uint32_t> direct_lookup(range);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        direct_lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    uint64_t sum = 0;
    const std::size_t n_probe = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            const uint32_t idx = probe_keys[i] - min_key;  // probe_key is guaranteed to be in [min_key, max_key]
            sum += static_cast<uint64_t>(direct_lookup[idx]) *
                   static_cast<uint64_t>(probe_weights[i]);
        }
    }

    return sum;
}
