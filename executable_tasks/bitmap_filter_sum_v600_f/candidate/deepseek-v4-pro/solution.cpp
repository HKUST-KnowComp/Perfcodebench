#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    if (iters <= 0) return 0;

    // Build a dense bitmap of enabled keys
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(num_words, 0);
    for (uint32_t k : enabled_keys) {
        if (k < key_space) {
            size_t idx = k / 64;
            size_t bit = k % 64;
            bitmap[idx] |= (1ULL << bit);
        }
    }

    // Single pass over probe_keys and values
    uint64_t sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t k = probe_keys[i];
        if (k < key_space && (bitmap[k / 64] & (1ULL << (k % 64)))) {
            sum += static_cast<uint64_t>(values[i]);
        }
    }
    return sum;
}