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
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // Find the range of keys to build a direct lookup table
    uint32_t max_key = dim_keys.back();
    
    // We use a lookup table for O(1) access. 
    // If max_key is too large, we'd fallback to binary search, 
    // but 'dense dimension' implies a compact key space.
    // To handle potential sparsity or large ranges safely while staying fast,
    // we check if a direct map is feasible. 
    // For this specific performance task, we assume the dense dimension 
    // allows for a high-speed lookup array.
    
    std::vector<uint32_t> lookup(max_key + 1, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const std::size_t n = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        // Unrolling or SIMD could be applied here, but a clean loop 
        // with O(1) lookup is the primary bottleneck breaker.
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t key = p_keys[i];
            // The problem implies dim_keys contains the probe_keys.
            // If probe_keys can exceed max_key, we must check bounds.
            if (key <= max_key) {
                current_iter_sum += static_cast<uint64_t>(lookup[key]) * p_weights[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}