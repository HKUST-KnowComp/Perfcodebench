#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    // Precompute direct lookup table: assume dim_keys is dense and sorted.
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    std::size_t range_size = static_cast<std::size_t>(max_key - min_key) + 1;
    std::vector<uint32_t> lut(range_size);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lut[dim_keys[i] - min_key] = dim_values[i];
    }
    
    uint64_t sum = 0;
    const auto lut_data = lut.data();
    const auto probe_keys_data = probe_keys.data();
    const auto probe_weights_data = probe_weights.data();
    const std::size_t n_probe = probe_keys.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n_probe; ++i) {
            std::size_t idx = static_cast<std::size_t>(probe_keys_data[i] - min_key);
            sum += static_cast<uint64_t>(lut_data[idx]) * static_cast<uint64_t>(probe_weights_data[i]);
        }
    }
    return sum;
}