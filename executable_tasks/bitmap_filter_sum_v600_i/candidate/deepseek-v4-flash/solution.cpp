#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    // Precompute bitmap
    std::vector<uint8_t> bitmap(key_space, 0);
    for (auto key : enabled_keys) {
        bitmap[key] = 1;
    }
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint8_t* bitmap_ptr = bitmap.data();
        const uint32_t* probe_ptr = probe_keys.data();
        const uint32_t* values_ptr = values.data();
        std::size_t n = probe_keys.size();
        for (std::size_t i = 0; i < n; ++i) {
            if (bitmap_ptr[probe_ptr[i]]) {
                sum += static_cast<uint64_t>(values_ptr[i]);
            }
        }
    }
    return sum;
}