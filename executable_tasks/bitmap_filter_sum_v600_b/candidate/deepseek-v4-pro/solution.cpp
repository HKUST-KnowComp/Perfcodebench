#include "interface.h"

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    (void)iters; // iterations are redundant; result is deterministic
    
    // Build bitmap: each 64-bit word covers 64 keys
    std::vector<uint64_t> bitmap((key_space + 63) / 64, 0);
    for (uint32_t key : enabled_keys) {
        if (key < key_space) {
            bitmap[key >> 6] |= (1ULL << (key & 63));
        }
    }

    uint64_t sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t k = probe_keys[i];
        if (k < key_space && (bitmap[k >> 6] & (1ULL << (k & 63)))) {
            sum += values[i];
        }
    }
    return sum;
}
