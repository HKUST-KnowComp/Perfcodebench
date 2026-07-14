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

    // Find the range of keys to determine if we can use a direct lookup table
    uint32_t min_key = dim_keys[0];
    uint32_t max_key = dim_keys[0];
    for (uint32_t k : dim_keys) {
        if (k < min_key) min_key = k;
        if (k > max_key) max_key = k;
    }

    size_t range = static_cast<size_t>(max_key - min_key) + 1;
    
    // If the range is manageable (e.g., fits in memory), use a direct lookup table.
    // A limit of 2^26 (approx 67 million entries) is a safe heuristic for typical memory constraints.
    if (range < (1 << 26)) {
        std::vector<uint32_t> lookup(range, 0);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i] - min_key] = dim_values[i];
        }

        uint64_t total_sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_iter_sum = 0;
            const uint32_t* pk = probe_keys.data();
            const uint32_t* pw = probe_weights.data();
            size_t n = probe_keys.size();
            
            for (size_t i = 0; i < n; ++i) {
                uint32_t k = pk[i];
                if (k >= min_key && k <= max_key) {
                    current_iter_sum += static_cast<uint64_t>(lookup[k - min_key]) * pw[i];
                }
            }
            total_sum += current_iter_sum;
        }
        // The task asks for the sum of the last iteration or the cumulative sum? 
        // The baseline returns the sum of the LAST iteration (it resets sum = 0 inside the loop).
        // Wait, the baseline: 'sum = 0;' is inside the 'iter' loop. So it returns the sum of the LAST iteration.
        // Let's re-read: 'sum = 0;' is inside 'for (int iter = 0; iter < iters; ++iter)'.
        // So it returns the sum of the very last iteration.
        
        // Re-calculating to match baseline behavior exactly (returning sum of the last iteration):
        uint64_t last_sum = 0;
        for (size_t i = 0; i < probe_keys.size(); ++i) {
            uint32_t k = probe_keys[i];
            if (k >= min_key && k <= max_key) {
                last_sum += static_cast<uint64_t>(lookup[k - min_key]) * probe_weights[i];
            }
        }
        return last_sum;
    } else {
        // Fallback to optimized binary search if range is too large
        uint64_t last_sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            last_sum = 0;
            for (size_t i = 0; i < probe_keys.size(); ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
                if (it != dim_keys.end() && *it == probe_keys[i]) {
                    size_t idx = std::distance(dim_keys.begin(), it);
                    last_sum += static_cast<uint64_t>(dim_values[idx]) * probe_weights[i];
                }
            }
        }
        return last_sum;
    }
}