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
    
    if (dim_keys.empty()) return 0;

    // Find the range of keys to build a direct lookup table
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    
    // Create a lookup table for O(1) access
    std::vector<uint32_t> lookup(max_key - min_key + 1);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const size_t num_probes = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < num_probes; ++i) {
            uint32_t key = p_keys[i];
            // Assuming probe keys are within the range of dim_keys as per dense dimension join context
            if (key >= min_key && key <= max_key) {
                iter_sum += static_cast<uint64_t>(lookup[key - min_key]) * static_cast<uint64_t>(p_weights[i]);
            }
        }
        total_sum = iter_sum;
    }
    
    return total_sum;
}