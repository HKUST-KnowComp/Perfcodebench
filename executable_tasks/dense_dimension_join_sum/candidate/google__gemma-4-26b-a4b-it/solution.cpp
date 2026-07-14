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

    // Find the range of keys to determine if a direct lookup table is feasible.
    // For a 'dense dimension', we assume the keys are either a contiguous range 
    // or can be mapped efficiently. 
    // To handle any uint32_t range while staying fast, we first check the spread.
    uint32_t min_key = dim_keys[0];
    uint32_t max_key = dim_keys[0];
    for (uint32_t k : dim_keys) {
        if (k < min_key) min_key = k;
        if (k > max_key) max_key = k;
    }

    uint64_t total_sum = 0;
    size_t num_probes = probe_keys.size();

    // If the range is small enough to fit in a direct-address table (e.g., < 2^24 elements)
    // we use a lookup table for O(1) access. 
    // 16M entries * 4 bytes = 64MB, which fits in RAM.
    if (max_key - min_key < 16777216) {
        std::vector<uint32_t> lookup(max_key - min_key + 1, 0);
        // Note: The problem implies dim_keys is sorted or we need to map them.
        // The baseline used lower_bound, implying dim_keys is sorted.
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i] - min_key] = dim_values[i];
        }

        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            const uint32_t* pk = probe_keys.data();
            const uint32_t* pw = probe_weights.data();
            for (size_t i = 0; i < num_probes; ++i) {
                uint32_t k = pk[i];
                // Check bounds to ensure correctness if probe_keys contains values outside dim range
                if (k >= min_key && k <= max_key) {
                    current_sum += static_cast<uint64_t>(lookup[k - min_key]) * pw[i];
                }
            }
            total_sum = current_sum;
        }
    } else {
        // Fallback to optimized binary search if range is too sparse/large
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            for (size_t i = 0; i < num_probes; ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
                if (it != dim_keys.end() && *it == probe_keys[i]) {
                    size_t idx = std::distance(dim_keys.begin(), it);
                    current_sum += static_cast<uint64_t>(dim_values[idx]) * probe_weights[i];
                }
            }
            total_sum = current_sum;
        }
    }

    return total_sum;
}