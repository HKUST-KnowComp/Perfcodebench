#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const uint32_t* __restrict__ d_vals = dim_values.data();
    const uint32_t* __restrict__ p_keys = probe_keys.data();
    const uint32_t* __restrict__ p_weights = probe_weights.data();
    const std::size_t n = probe_keys.size();
    const uint32_t offset = dim_keys.empty() ? 0 : dim_keys[0];
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        std::size_t i = 0;
        for (; i + 3 < n; i += 4) {
            s0 += static_cast<uint64_t>(d_vals[p_keys[i] - offset]) * p_weights[i];
            s1 += static_cast<uint64_t>(d_vals[p_keys[i+1] - offset]) * p_weights[i+1];
            s2 += static_cast<uint64_t>(d_vals[p_keys[i+2] - offset]) * p_weights[i+2];
            s3 += static_cast<uint64_t>(d_vals[p_keys[i+3] - offset]) * p_weights[i+3];
        }
        for (; i < n; ++i) {
            s0 += static_cast<uint64_t>(d_vals[p_keys[i] - offset]) * p_weights[i];
        }
        sum = s0 + s1 + s2 + s3;
    }
    return sum;
}