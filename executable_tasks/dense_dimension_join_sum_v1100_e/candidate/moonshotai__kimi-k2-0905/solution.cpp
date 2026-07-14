#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build direct lookup table once: value_of[key] = dim_values[index_of_key]
    const uint32_t max_key = dim_keys.back();
    std::vector<uint32_t> value_of(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        value_of[dim_keys[i]] = dim_values[i];
    }

    const size_t n = probe_keys.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            sum += static_cast<uint64_t>(value_of[probe_keys[i]]) *
                   static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}