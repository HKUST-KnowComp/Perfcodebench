#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    // one bit per key; 64-bit words
    const uint32_t n_words = (key_space + 63) / 64;
    std::vector<uint64_t> bits(n_words, 0);

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // clear bitmap
        std::memset(bits.data(), 0, n_words * sizeof(uint64_t));

        // mark enabled keys
        for (uint32_t k : enabled_keys) {
            const uint32_t word = k >> 6;
            const uint32_t bit  = k & 63;
            bits[word] |= (1ULL << bit);
        }

        // probe and accumulate
        const size_t n = probe_keys.size();
        uint64_t local_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint32_t word = k >> 6;
            const uint32_t bit  = k & 63;
            if ((bits[word] >> bit) & 1) {
                local_sum += static_cast<uint64_t>(values[i]);
            }
        }
        sum = local_sum;
    }
    return sum;
}