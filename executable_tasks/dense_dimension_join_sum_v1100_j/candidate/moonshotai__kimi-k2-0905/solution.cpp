#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    // Build a direct lookup table: key -> value (O(1) access)
    const uint32_t max_key = dim_keys.empty() ? 0 : dim_keys.back();
    std::vector<uint32_t> map(max_key + 1, 0);
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        map[dim_keys[i]] = dim_values[i];
    }

    const uint32_t* __restrict map_ptr = map.data();
    const uint32_t* __restrict pk  = probe_keys.data();
    const uint32_t* __restrict pw  = probe_weights.data();
    const size_t n = probe_keys.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        size_t i = 0;
        // Unroll 8× to reduce loop overhead and increase ILP
        for (; i + 8 <= n; i += 8) {
            local_sum += uint64_t(map_ptr[pk[i  ]]) * uint64_t(pw[i  ]);
            local_sum += uint64_t(map_ptr[pk[i+1]]) * uint64_t(pw[i+1]);
            local_sum += uint64_t(map_ptr[pk[i+2]]) * uint64_t(pw[i+2]);
            local_sum += uint64_t(map_ptr[pk[i+3]]) * uint64_t(pw[i+3]);
            local_sum += uint64_t(map_ptr[pk[i+4]]) * uint64_t(pw[i+4]);
            local_sum += uint64_t(map_ptr[pk[i+5]]) * uint64_t(pw[i+5]);
            local_sum += uint64_t(map_ptr[pk[i+6]]) * uint64_t(pw[i+6]);
            local_sum += uint64_t(map_ptr[pk[i+7]]) * uint64_t(pw[i+7]);
        }
        // Tail
        for (; i < n; ++i) {
            local_sum += uint64_t(map_ptr[pk[i]]) * uint64_t(pw[i]);
        }
        sum = local_sum;
    }
    return sum;
}