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

    // Find the range of keys to determine if a direct map is feasible
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) if (k > max_key) max_key = k;
    for (uint32_t k : probe_keys) if (k > max_key) max_key = k;

    // If the key range is reasonably small, use a direct lookup table
    // Otherwise, we would use a hash map or keep the binary search.
    // For 'dense' dimensions, a direct map is usually the intended optimization.
    if (max_key < 10000000) {
        std::vector<uint32_t> lookup(max_key + 1, 0);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i]] = dim_values[i];
        }

        uint64_t total_sum = 0;
        const size_t probe_size = probe_keys.size();
        
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                current_sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * probe_weights[i];
            }
            total_sum = current_sum;
        }
        return total_sum;
    } else {
        // Fallback for very sparse/large keys: binary search is O(log N)
        // but we can optimize by using raw pointers to avoid vector overhead
        const uint32_t* d_keys = dim_keys.data();
        const uint32_t* d_vals = dim_values.data();
        const uint32_t* p_keys = probe_keys.data();
        const uint32_t* p_weights = probe_weights.data();
        size_t d_size = dim_keys.size();
        size_t p_size = probe_keys.size();

        uint64_t total_sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            for (size_t i = 0; i < p_size; ++i) {
                uint32_t target = p_keys[i];
                auto it = std::lower_bound(d_keys, d_keys + d_size, target);
                size_t idx = it - d_keys;
                current_sum += static_cast<uint64_t>(d_vals[idx]) * p_weights[i];
            }
            total_sum = current_sum;
        }
        return total_sum;
    }
}