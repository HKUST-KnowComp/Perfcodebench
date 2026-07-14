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

    // Find the range of keys to determine if we can use a direct map
    uint32_t max_key = dim_keys.back();
    
    // To avoid massive memory allocation if keys are sparse but large, 
    // we check if the dimension table is truly dense relative to the max key.
    // However, the task title "dense dimension join" implies we can map keys to indices.
    // We create a lookup table where lookup[key] = value.
    
    std::vector<uint32_t> lookup(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* p_lookup = lookup.data();
    const size_t n_probe = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n_probe; ++i) {
            // Direct lookup replaces std::lower_bound
            current_sum += static_cast<uint64_t>(p_lookup[p_keys[i]]) * static_cast<uint64_t>(p_weights[i]);
        }
        total_sum = current_sum;
    }

    return total_sum;
}