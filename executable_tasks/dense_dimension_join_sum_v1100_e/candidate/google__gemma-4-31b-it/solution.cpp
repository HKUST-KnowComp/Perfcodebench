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

    // Find the range of keys to determine the size of the lookup table
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) if (k > max_key) max_key = k;
    for (uint32_t k : probe_keys) if (k > max_key) max_key = k;

    // Create a lookup table for O(1) access. 
    // Using a vector for safety, though a raw array might be slightly faster.
    // We initialize with 0. If dim_keys is dense and sorted, we can map directly.
    std::vector<uint32_t> lookup(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t final_sum = 0;
    const size_t probe_size = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* l_table = lookup.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < probe_size; ++i) {
            current_sum += static_cast<uint64_t>(l_table[p_keys[i]]) * p_weights[i];
        }
        final_sum = current_sum;
    }

    return final_sum;
}