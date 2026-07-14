#include "interface.h"

#include <cstring>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    // Allocate bitset once; size rounded up to 64-bit words
    const uint32_t words = (key_space + 63) / 64;
    std::vector<uint64_t> bitmap(words, 0);

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Fast clear
        std::memset(bitmap.data(), 0, words * sizeof(uint64_t));

        // Populate bitmap from enabled_keys
        for (uint32_t k : enabled_keys) {
            const uint32_t idx = k / 64;
            const uint32_t bit = k % 64;
            bitmap[idx] |= (1ULL << bit);
        }

        // Scan probe_keys and accumulate
        const std::size_t n = probe_keys.size();
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            const uint32_t idx = k / 64;
            const uint32_t bit = k % 64;
            if ((bitmap[idx] >> bit) & 1ULL) {
                local_sum += static_cast<uint64_t>(values[i]);
            }
        }
        sum = local_sum;  // final value after last iter
    }

    return sum;
}