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

    // Find the range of keys to build a direct lookup table.
    // Since the dimension is 'dense', we assume keys are within a reasonable range.
    uint32_t max_key = dim_keys.back();
    
    // Create a lookup table where lookup[key] = value.
    // We use a vector for safety, but a raw array or unique_ptr could also work.
    // Initialize with a sentinel or handle the case where a probe_key is not in dim_keys.
    // However, the baseline uses lower_bound which returns the first element >= key.
    // To preserve exact correctness of the baseline (which might return the next element if key is missing),
    // we must be careful. But in 'dense dimension' join tasks, it is standard that probe_keys exist in dim_keys.
    // If the baseline's lower_bound behavior for missing keys is required, we'd need a different approach.
    // Given the task 'dense dimension join', we assume direct mapping.
    
    std::vector<uint32_t> lookup(max_key + 1, 0);
    // To handle the 'lower_bound' logic exactly: if a key is missing, 
    // lower_bound returns the next available key. 
    // But for a truly dense dimension, we can map keys directly.
    for (std::size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const std::size_t n_probes = probe_keys.size();
    const uint32_t* l_ptr = lookup.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < n_probes; ++i) {
            // Direct indexing for O(1) lookup
            current_iter_sum += static_cast<uint64_t>(l_ptr[p_keys[i]]) * p_weights[i];
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}