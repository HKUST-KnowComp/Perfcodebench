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

    // Since the dimension is 'dense', we can build a direct lookup table.
    // We find the range of keys to determine the size of our lookup array.
    uint32_t min_key = dim_keys.front();
    uint32_t max_key = dim_keys.back();
    uint32_t range = max_key - min_key + 1;

    // Use a vector for the lookup table to handle the mapping from key to value.
    // We initialize with 0. If a key is not in dim_keys, its value is 0.
    // Note: The problem implies dim_keys are sorted and represent a dense dimension.
    std::vector<uint32_t> lookup(range, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const std::size_t n = probe_keys.size();
    const uint32_t* l_ptr = lookup.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t key = p_keys[i];
            // Check if key is within the range of our dense dimension
            if (key >= min_key && key <= max_key) {
                current_iter_sum += static_cast<uint64_t>(l_ptr[key - min_key]) * p_weights[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}