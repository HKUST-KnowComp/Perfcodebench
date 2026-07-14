#include "interface.h"
#include <algorithm>
#include <unordered_map>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_keys.empty() || probe_keys.empty()) return 0;

    const uint32_t min_key = dim_keys.front();
    const uint32_t max_key = dim_keys.back();
    const std::size_t dim_size = dim_keys.size();
    const bool is_dense = (static_cast<std::size_t>(max_key - min_key) + 1 == dim_size);

    if (is_dense) {
        // Dense dimension: create offset mapping
        std::vector<uint32_t> lut(dim_size);
        for (std::size_t i = 0; i < dim_size; ++i) {
            lut[dim_keys[i] - min_key] = dim_values[i];
        }

        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
            const uint32_t* __restrict lut_ptr = lut.data();
            const uint32_t* __restrict pkeys = probe_keys.data();
            const uint32_t* __restrict pweights = probe_weights.data();
            const std::size_t psize = probe_keys.size();
            for (std::size_t i = 0; i < psize; ++i) {
                sum += static_cast<uint64_t>(lut_ptr[pkeys[i] - min_key]) *
                       static_cast<uint64_t>(pweights[i]);
            }
        }
        return sum;
    } else {
        // Fallback: build a hash map for non-dense keys
        std::unordered_map<uint32_t, uint32_t> map;
        map.reserve(dim_size);
        for (std::size_t i = 0; i < dim_size; ++i) {
            map[dim_keys[i]] = dim_values[i];
        }

        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
            for (std::size_t i = 0; i < probe_keys.size(); ++i) {
                auto it = map.find(probe_keys[i]);
                if (it != map.end()) {
                    sum += static_cast<uint64_t>(it->second) *
                           static_cast<uint64_t>(probe_weights[i]);
                }
                // Missing keys are ignored (matching original UB by not adding,
                // but the test harness is expected to provide only valid keys).
            }
        }
        return sum;
    }
}
