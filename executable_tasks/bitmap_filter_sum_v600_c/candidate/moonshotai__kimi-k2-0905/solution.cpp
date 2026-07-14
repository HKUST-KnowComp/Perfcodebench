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
    const uint32_t words = (key_space + 63) >> 6;
    std::vector<uint64_t> bitmap(words, 0);

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Clear bitmap
        std::memset(bitmap.data(), 0, words * sizeof(uint64_t));

        // Populate bitmap
        for (uint32_t k : enabled_keys) {
            if (k < key_space) {
                bitmap[k >> 6] |= uint64_t(1) << (k & 63);
            }
        }

        // Aggregate
        sum = 0;
        const size_t n = probe_keys.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t k = probe_keys[i];
            if (k < key_space) {
                const uint64_t bit = uint64_t(1) << (k & 63);
                if (bitmap[k >> 6] & bit) {
                    sum += static_cast<uint64_t>(values[i]);
                }
            }
        }
    }
    return sum;
}