#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    if (key_space == 0) return 0;
    size_t words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(words, 0);
    
    // Build bitmap once; enabled_keys never change across iters
    for (uint32_t k : enabled_keys) {
        bitmap[k >> 6] |= (uint64_t(1) << (k & 63));
    }
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t k = probe_keys[i];
            if ((bitmap[k >> 6] >> (k & 63)) & 1U) {
                sum += static_cast<uint64_t>(values[i]);
            }
        }
    }
    return sum;
}