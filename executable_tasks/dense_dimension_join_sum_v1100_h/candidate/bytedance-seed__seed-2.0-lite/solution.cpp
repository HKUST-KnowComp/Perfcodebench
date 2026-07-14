#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t sum = 0;
    if (dim_keys.empty() || probe_keys.empty()) return sum;

    // Build single dense lookup table for all dimension keys, leveraging dense dimension property
    const uint32_t base_key = dim_keys.front();
    const uint32_t max_key = dim_keys.back();
    std::vector<uint32_t> value_map(max_key - base_key + 1);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        const uint32_t k = dim_keys[i];
        value_map[k - base_key] = dim_values[i];
    }

    const size_t num_probes = probe_keys.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < num_probes; ++i) {
            const uint32_t probe_k = probe_keys[i];
            const uint32_t dim_val = value_map[probe_k - base_key];
            sum += static_cast<uint64_t>(dim_val) * static_cast<uint64_t>(probe_weights[i]);
        }
    }
    return sum;
}