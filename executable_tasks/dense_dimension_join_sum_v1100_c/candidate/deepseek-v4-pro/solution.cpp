#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (probe_keys.empty() || dim_keys.empty()) {
        return 0;
    }
    const uint32_t min_key = dim_keys.front();
    const uint32_t max_key = dim_keys.back();
    const size_t lut_size = static_cast<size_t>(max_key - min_key) + 1;
    std::vector<uint32_t> lut(lut_size);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lut[static_cast<size_t>(dim_keys[i] - min_key)] = dim_values[i];
    }
    const size_t num_probes = probe_keys.size();
    std::vector<uint64_t> products(num_probes);
    for (size_t i = 0; i < num_probes; ++i) {
        const uint32_t val = lut[static_cast<size_t>(probe_keys[i] - min_key)];
        products[i] = static_cast<uint64_t>(val) * static_cast<uint64_t>(probe_weights[i]);
    }
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < num_probes; ++i) {
            sum += products[i];
        }
    }
    return sum;
}