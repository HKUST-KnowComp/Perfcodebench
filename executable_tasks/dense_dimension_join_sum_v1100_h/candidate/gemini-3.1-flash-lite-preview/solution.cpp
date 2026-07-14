#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    // Find the range of keys to build a direct lookup table
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) {
        if (k > max_key) max_key = k;
    }

    // Build LUT: index is key, value is dim_value
    // Using a vector initialized to 0. If keys are sparse, this might be large,
    // but for 'dense dimension' tasks, this is the optimal O(1) lookup strategy.
    std::vector<uint32_t> lut(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lut[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const size_t num_probes = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* lut_ptr = lut.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < num_probes; ++i) {
            // Direct lookup is O(1) and branchless
            iter_sum += static_cast<uint64_t>(lut_ptr[p_keys[i]]) * static_cast<uint64_t>(p_weights[i]);
        }
        total_sum = iter_sum;
    }

    return total_sum;
}