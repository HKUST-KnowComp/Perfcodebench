#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    const uint32_t* lv = nullptr;
    std::vector<uint32_t> lookup;
    
    bool is_identity = (dim_keys.size() == dim_values.size());
    if (is_identity) {
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            if (dim_keys[i] != i) {
                is_identity = false;
                break;
            }
        }
    }
    
    if (is_identity) {
        lv = dim_values.data();
    } else {
        uint32_t max_key = dim_keys.empty() ? 0 : dim_keys.back();
        lookup.resize(max_key + 1);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lookup[dim_keys[i]] = dim_values[i];
        }
        lv = lookup.data();
    }

    const size_t n = probe_keys.size();
    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            s0 += static_cast<uint64_t>(lv[pk[i]]) * pw[i];
            s1 += static_cast<uint64_t>(lv[pk[i+1]]) * pw[i+1];
            s2 += static_cast<uint64_t>(lv[pk[i+2]]) * pw[i+2];
            s3 += static_cast<uint64_t>(lv[pk[i+3]]) * pw[i+3];
        }
        for (; i < n; ++i) {
            s0 += static_cast<uint64_t>(lv[pk[i]]) * pw[i];
        }
        sum = s0 + s1 + s2 + s3;
    }
    return sum;
}
