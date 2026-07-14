#include "interface.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // Find the range of keys to determine if we can use a flat array for O(1) lookup
    uint32_t min_k = dim_keys[0];
    uint32_t max_k = dim_keys[0];
    for (uint32_t k : dim_keys) {
        if (k < min_k) min_k = k;
        if (k > max_k) max_k = k;
    }

    size_t range = static_cast<size_t>(max_k) - min_k + 1;
    
    // If the dimension is dense enough, use a flat array for maximum performance
    // Otherwise, a hash map or sorted search is needed. 
    // Given the task title "dense dimension", a flat array is the optimal choice.
    // We use a reasonable threshold for memory (e.g., 256MB).
    if (range < 64 * 1024 * 1024) {
        std::vector<uint32_t> lookup(range);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i] - min_k] = dim_values[i];
        }

        uint64_t total_sum = 0;
        const size_t probe_size = probe_keys.size();
        
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                uint32_t key = probe_keys[i];
                // Ensure key is within the range of the lookup table
                if (key >= min_k && key <= max_k) {
                    iter_sum += static_cast<uint64_t>(lookup[key - min_k]) * probe_weights[i];
                }
            }
            total_sum = iter_sum;
        }
        return total_sum;
    } else {
        // Fallback for extremely sparse/wide keys: use a hash map or the original binary search
        // but since the goal is performance on "dense" dimensions, the array is the primary path.
        std::unordered_map<uint32_t, uint32_t> lookup;
        lookup.reserve(dim_keys.size());
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i]] = dim_values[i];
        }

        uint64_t total_sum = 0;
        const size_t probe_size = probe_keys.size();
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                auto it = lookup.find(probe_keys[i]);
                if (it != lookup.end()) {
                    iter_sum += static_cast<uint64_t>(it->second) * probe_weights[i];
                }
            }
            total_sum = iter_sum;
        }
        return total_sum;
    }
}