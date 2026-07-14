#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    std::vector<uint8_t> bitmap(key_space, 0);
    for (uint32_t k : enabled_keys) {
        bitmap[k] = 1;
    }

    uint64_t sum = 0;
    const uint32_t* pk = probe_keys.data();
    const uint32_t* vals = values.data();
    size_t n = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (bitmap[pk[i]]) {
                current_sum += vals[i];
            }
        }
        sum = current_sum;
    }
    return sum;
}