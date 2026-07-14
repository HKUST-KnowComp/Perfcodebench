#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build a dense lookup table once: value_by_key[key] == dim_values[i]
    // The benchmark guarantees keys are dense, so we can safely size the table
    // to max_key+1.
    uint32_t max_key = 0;
    if (!dim_keys.empty()) {
        max_key = *std::max_element(dim_keys.begin(), dim_keys.end());
    }
    std::vector<uint32_t> value_by_key(max_key + 1, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        value_by_key[dim_keys[i]] = dim_values[i];
    }

    uint64_t sum = 0;
    const std::size_t n = probe_keys.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint64_t v = value_by_key[k];
            const uint64_t w = probe_weights[i];
            sum += v * w;
        }
    }
    return sum;
}