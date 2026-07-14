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
    uint32_t max_key = dim_keys.back();
    
    // Create a lookup table for O(1) access.
    // We use a vector to handle potential large key ranges safely.
    // If max_key is extremely large, a hash map or a hybrid approach would be needed,
    // but for 'dense dimension' tasks, a LUT is the standard optimization.
    std::vector<uint32_t> lut(max_key + 1, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lut[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const std::size_t n = probe_keys.size();
    const uint32_t* lut_ptr = lut.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        // Unrolling or SIMD could be applied here, but a clean loop
        // with direct indexing is usually sufficient for modern compilers.
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t key = p_keys[i];
            // Based on the problem description, dim_keys is sorted and dense.
            // We assume probe_keys are within the range of dim_keys.
            current_iter_sum += static_cast<uint64_t>(lut_ptr[key]) * p_weights[i];
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}