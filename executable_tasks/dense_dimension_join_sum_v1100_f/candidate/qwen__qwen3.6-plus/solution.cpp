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

    uint32_t min_k = dim_keys.front();
    uint32_t max_k = dim_keys.back();
    uint32_t range = max_k - min_k + 1;
    
    // Fallback to binary search if key range is too large to avoid excessive memory allocation
    if (range > 20000000) {
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t iter_sum = 0;
            for (size_t i = 0; i < probe_keys.size(); ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
                iter_sum += (uint64_t)dim_values[it - dim_keys.begin()] * probe_weights[i];
            }
            sum = iter_sum;
        }
        return sum;
    }

    std::vector<uint32_t> lookup(range, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i] - min_k] = dim_values[i];
    }

    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* p_lookup = lookup.data();
    size_t n = probe_keys.size();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            iter_sum += (uint64_t)p_lookup[p_keys[i] - min_k] * p_weights[i];
        }
        sum = iter_sum;
    }
    return sum;
}