#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const uint32_t* d_keys = dim_keys.data();
    const uint32_t* d_vals = dim_values.data();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    
    const size_t n = probe_keys.size();
    const size_t dim_n = dim_keys.size();
    
    // Detect if dimension keys form a contiguous dense range
    const bool is_dense = (dim_n > 0 && d_keys[dim_n - 1] == d_keys[0] + dim_n - 1);
    const uint32_t offset = is_dense ? d_keys[0] : 0;
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        if (is_dense) {
            // O(1) direct lookup enables auto-vectorization and eliminates branch misses
            for (size_t i = 0; i < n; ++i) {
                sum += static_cast<uint64_t>(d_vals[p_keys[i] - offset]) * static_cast<uint64_t>(p_weights[i]);
            }
        } else {
            // Fallback for non-dense dimensions preserves correctness
            for (size_t i = 0; i < n; ++i) {
                const uint32_t* it = std::lower_bound(d_keys, d_keys + dim_n, p_keys[i]);
                const size_t idx = static_cast<size_t>(it - d_keys);
                sum += static_cast<uint64_t>(d_vals[idx]) * static_cast<uint64_t>(p_weights[i]);
            }
        }
    }
    return sum;
}