#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (key_space == 0 || enabled_keys.empty() || probe_keys.empty()) {
        return 0;
    }

    std::vector<uint8_t> is_enabled(key_space, 0);
    for (uint32_t k : enabled_keys) {
        is_enabled[k] = 1;
    }

    const uint32_t* pk = probe_keys.data();
    const uint32_t* vals = values.data();
    const size_t n = probe_keys.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            current_sum += static_cast<uint64_t>(vals[i]) * is_enabled[pk[i]];
        }
        sum = current_sum;
    }
    return sum;
}