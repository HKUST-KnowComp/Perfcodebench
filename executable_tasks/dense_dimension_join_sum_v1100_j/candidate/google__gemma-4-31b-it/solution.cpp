#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // Find the range of keys to determine if we can use a direct map
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    
    // For dense dimensions, a lookup table is significantly faster than binary search.
    // We check if the range is reasonable to allocate. 
    // If the range is too large, we'd fall back to binary search, but the task 
    // specifies 'dense dimension', implying a compact key range.
    size_t range = static_cast<size_t>(max_key) - min_key + 1;
    
    // Heuristic: if range is within a reasonable memory limit (e.g., 256MB), use lookup table
    if (range < 64 * 1024 * 1024) {
        std::vector<uint32_t> lookup(range);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i] - min_key] = dim_values[i];
        }

        uint64_t total_sum = 0;
        const uint32_t* p_keys = probe_keys.data();
        const uint32_t* p_weights = probe_weights.data();
        const uint32_t* l_table = lookup.data();
        size_t probe_size = probe_keys.size();

        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                uint32_t key = p_keys[i];
                // Assuming probe_keys are within the range of dim_keys based on 'join' context
                iter_sum += static_cast<uint64_t>(l_table[key - min_key]) * p_weights[i];
            }
            total_sum = iter_sum;
        }
        return total_sum;
    } else {
        // Fallback to optimized binary search if range is too sparse/large
        uint64_t total_sum = 0;
        size_t probe_size = probe_keys.size();
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
                size_t idx = std::distance(dim_keys.begin(), it);
                iter_sum += static_cast<uint64_t>(dim_values[idx]) * probe_weights[i];
            }
            total_sum = iter_sum;
        }
        return total_sum;
    }
}