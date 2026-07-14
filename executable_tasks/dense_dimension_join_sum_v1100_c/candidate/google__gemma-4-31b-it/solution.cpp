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

    // Determine the range of keys to see if we can use a flat array for O(1) access
    // If the keys are very sparse, a hash map is safer, but for 'dense' dimensions,
    // a flat array or a sorted lookup is usually intended. 
    // However, since we don't know the max key value, we check the range.
    uint32_t min_k = dim_keys[0];
    uint32_t max_k = dim_keys[0];
    for (uint32_t k : dim_keys) {
        if (k < min_k) min_k = k;
        if (k > max_k) max_k = k;
    }

    size_t range = static_cast<size_t>(max_k) - min_k + 1;
    
    // Heuristic: if the range is reasonably small (e.g., < 10M elements), use a flat array
    if (range < 10000000) {
        std::vector<uint32_t> lookup(range, 0);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i] - min_k] = dim_values[i];
        }

        uint64_t total_sum = 0;
        const size_t probe_size = probe_keys.size();
        
        // Pre-calculate the sum for one iteration to avoid redundant lookups
        uint64_t single_iter_sum = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t pk = probe_keys[i];
            if (pk >= min_k && pk <= max_k) {
                single_iter_sum += static_cast<uint64_t>(lookup[pk - min_k]) * probe_weights[i];
            }
        }
        
        return single_iter_sum * iters;
    } else {
        // Fallback for very sparse/large keys: use a hash map or keep binary search
        // Given the 'dense' hint, the array path is the primary optimization.
        std::unordered_map<uint32_t, uint32_t> lookup;
        lookup.reserve(dim_keys.size());
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i]] = dim_values[i];
        }

        uint64_t single_iter_sum = 0;
        for (size_t i = 0; i < probe_keys.size(); ++i) {
            auto it = lookup.find(probe_keys[i]);
            if (it != lookup.end()) {
                single_iter_sum += static_cast<uint64_t>(it->second) * probe_weights[i];
            }
        }
        return single_iter_sum * iters;
    }
}