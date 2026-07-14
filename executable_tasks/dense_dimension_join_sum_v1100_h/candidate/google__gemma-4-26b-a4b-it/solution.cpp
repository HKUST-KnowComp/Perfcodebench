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

    // Find the range of keys to build a direct lookup table.
    // Given the 'dense dimension' hint, a LUT is highly efficient.
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) {
        if (k > max_key) max_key = k;
    }

    // Create a lookup table. We use a vector to handle potential sparsity 
    // or large ranges, but for a 'dense' dimension, this is optimal.
    // We initialize with 0. If a key is not in dim_keys, the join result 
    // for that key is effectively 0 (assuming dim_keys is sorted and unique).
    std::vector<uint32_t> lut(max_key + 1, 0);
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lut[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const std::size_t num_probes = probe_keys.size();n
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < num_probes; ++i) {
            uint32_t key = probe_keys[i];
            // Only probe if the key is within the range of our LUT
            if (key <= max_key) {
                current_iter_sum += static_cast<uint64_t>(lut[key]) * probe_weights[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}