#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    const uint32_t n_bits = key_space;
    const uint32_t n_words = (n_bits + 63) / 64;
    std::vector<uint64_t> bitmap(n_words, 0);

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // clear bitmap in bulk
        std::fill(bitmap.begin(), bitmap.end(), 0);

        // set enabled bits
        for (uint32_t k : enabled_keys) {
            const uint32_t word_idx = k >> 6;
            const uint32_t bit_idx  = k & 63;
            bitmap[word_idx] |= (1ULL << bit_idx);
        }

        // aggregate
        sum = 0;
        const size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint32_t word_idx = k >> 6;
            const uint32_t bit_idx  = k & 63;
            const uint64_t mask = 1ULL << bit_idx;
            if (bitmap[word_idx] & mask) {
                sum += static_cast<uint64_t>(values[i]);
            }
        }
    }
    return sum;
}