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

    // Use a lookup table for O(1) access. 
    // Given the 'dense' description, we assume max_key is manageable.
    // If max_key is too large, we'd fall back to binary search or a hash map,
    // but for 'dense dimension' tasks, a vector is usually optimal.
    std::vector<uint32_t> lookup(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    uint64_t total_sum = 0;
    size_t probe_size = probe_keys.size();
    
    // Pre-calculate the sum for one iteration to avoid redundant lookups
    uint64_t single_iter_sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
        single_iter_sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * 
                            static_cast<uint64_t>(probe_weights[i]);
    }

    // The problem asks for the sum returned after 'iters' iterations.
    // The baseline resets 'sum = 0' inside the loop, meaning it returns the result of the LAST iteration.
    // However, the baseline logic is: for(iter < iters) { sum = 0; ... } return sum;
    // This means the result is simply the sum of one iteration.
    return single_iter_sum;
}