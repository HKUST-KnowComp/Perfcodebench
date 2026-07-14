#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    // Allocate bitset once; 64-bit granularity for fast clearing.
    const uint32_t n_bits   = key_space;
    const uint32_t n_words  = (n_bits + 63) / 64;
    std::vector<uint64_t> bitset(n_words, 0);

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Fast clear: 64-bit stores.
        for (uint32_t w = 0; w < n_words; ++w) bitset[w] = 0;

        // Mark enabled keys.
        for (uint32_t k : enabled_keys) {
            const uint32_t word = k >> 6;
            const uint32_t bit  = k & 63;
            bitset[word] |= (1ULL << bit);
        }

        // Probe and accumulate.
        const std::size_t n = probe_keys.size();
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint32_t word = k >> 6;
            const uint32_t bit  = k & 63;
            const uint64_t mask = 1ULL << bit;
            if (bitset[word] & mask) {
                local_sum += static_cast<uint64_t>(values[i]);
            }
        }
        sum = local_sum;
    }
    return sum;
}