#include "interface.h"
#include <vector>
#include <unordered_map>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    // The problem states "dense dimension". 
    // We pre-calculate the mapping from key to value to avoid O(log N) search in the loop.
    // Since we don't know the exact range of keys, we check if they are contiguous/small.
    // For a general high-performance approach, we use a flat array if the max key is reasonable,
    // otherwise a hash map. However, for "dense" dimensions in performance tasks, 
    // usually a direct index or a sorted array with a fast lookup is intended.
    
    // Optimization: Pre-calculate the lookup table once outside the 'iters' loop.
    // We use a vector as a lookup table if the range is small, or a sorted array with 
    // a more efficient search. Given the "dense" hint, we'll use a simple mapping.
    
    size_t dim_size = dim_keys.size();
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) if (k > max_key) max_key = k;

    // If the keys are dense enough to fit in memory (e.g., < 100M elements),
    // a direct mapping array is the fastest possible lookup (O(1)).
    // 100M uint32_t is ~400MB, which is typically acceptable in these environments.
    bool use_direct_map = (max_key < 100000000);
    std::vector<uint32_t> lookup_table;
    if (use_direct_map) {
        lookup_table.assign(max_key + 1, 0);
        for (size_t i = 0; i < dim_size; ++i) {
            lookup_table[dim_keys[i]] = dim_values[i];
        }
    }

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    size_t probe_size = probe_keys.size();

    if (use_direct_map) {
        const uint32_t* l_table = lookup_table.data();
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                iter_sum += static_cast<uint64_t>(l_table[p_keys[i]]) * p_weights[i];
            }
            total_sum = iter_sum;
        }
    } else {
        // Fallback to binary search if keys are too sparse for a direct map,
        // but we can still optimize the loop.
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_size; ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), p_keys[i]);
                size_t idx = std::distance(dim_keys.begin(), it);
                iter_sum += static_cast<uint64_t>(dim_values[idx]) * p_weights[i];
            }
            total_sum = iter_sum;
        }
    }

    return total_sum;
}