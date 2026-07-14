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

    // Dimension keys are sorted (implied by baseline lower_bound usage).
    // Build a direct lookup table for O(1) access.
    uint32_t max_key = dim_keys.back();
    std::vector<uint32_t> lookup(max_key + 1);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* l_data = lookup.data();
    size_t n = probe_keys.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        size_t i = 0;
        // Unroll 4x to break dependency chains and improve ILP
        for (; i + 3 < n; i += 4) {
            s0 += static_cast<uint64_t>(l_data[p_keys[i]]) * p_weights[i];
            s1 += static_cast<uint64_t>(l_data[p_keys[i+1]]) * p_weights[i+1];
            s2 += static_cast<uint64_t>(l_data[p_keys[i+2]]) * p_weights[i+2];
            s3 += static_cast<uint64_t>(l_data[p_keys[i+3]]) * p_weights[i+3];
        }
        for (; i < n; ++i) {
            s0 += static_cast<uint64_t>(l_data[p_keys[i]]) * p_weights[i];
        }
        sum = s0 + s1 + s2 + s3;
    }
    return sum;
}
