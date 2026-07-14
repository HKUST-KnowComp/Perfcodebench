#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    uint32_t min_k = dim_keys.front();
    uint32_t max_k = dim_keys.back();
    uint64_t range = (uint64_t)max_k - min_k + 1;

    // Direct lookup table for dense keys. Threshold prevents OOM on sparse data.
    if (range < 64'000'000) {
        std::vector<uint32_t> lut(range);
        for (size_t i = 0; i < dim_keys.size(); ++i) {
            lut[dim_keys[i] - min_k] = dim_values[i];
        }

        const uint32_t* pk = probe_keys.data();
        const uint32_t* pw = probe_weights.data();
        const uint32_t* lt = lut.data();
        size_t n = probe_keys.size();
        uint64_t sum = 0;

        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
            for (size_t i = 0; i < n; ++i) {
                sum += (uint64_t)lt[pk[i] - min_k] * pw[i];
            }
        }
        return sum;
    } else {
        // Fallback to binary search for unexpectedly sparse/large key ranges
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
            for (size_t i = 0; i < probe_keys.size(); ++i) {
                auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
                size_t idx = (size_t)(it - dim_keys.begin());
                sum += (uint64_t)dim_values[idx] * probe_weights[i];
            }
        }
        return sum;
    }
}
