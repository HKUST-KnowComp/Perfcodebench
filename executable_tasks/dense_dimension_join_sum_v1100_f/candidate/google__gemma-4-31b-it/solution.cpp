#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty()) return 0;

    // Find the range of keys to determine the size of the lookup table
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    
    // Create a dense lookup table for O(1) access
    // The problem states 'dense dimension', implying the range is manageable
    std::vector<uint32_t> lookup(max_key - min_key + 1);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const size_t probe_size = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* l_ptr = lookup.data();
    const uint32_t offset = min_key;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            // Direct index access instead of binary search
            iter_sum += static_cast<uint64_t>(l_ptr[p_keys[i] - offset]) * 
                         static_cast<uint64_t>(p_weights[i]);
        }
        total_sum = iter_sum;
    }

    return total_sum;
}