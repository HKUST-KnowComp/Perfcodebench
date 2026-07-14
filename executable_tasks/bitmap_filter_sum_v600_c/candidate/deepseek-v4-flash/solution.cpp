#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    std::vector<bool> bitmap(key_space, false);
    for (uint32_t k : enabled_keys) {
        bitmap[k] = true;
    }
    uint64_t sum = 0;
    std::size_t n = probe_keys.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (bitmap[probe_keys[i]]) {
                sum += static_cast<uint64_t>(values[i]);
            }
        }
    }
    return sum;
}