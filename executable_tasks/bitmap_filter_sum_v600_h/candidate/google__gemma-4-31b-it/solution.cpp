#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    if (iters <= 0) return 0;

    // Use a bitset for O(1) lookup. 
    // key_space is the upper bound for keys.
    size_t bitmap_size = (key_space + 63) / 64;
    std::vector<uint64_t> bitmap(bitmap_size, 0);

    uint64_t total_sum = 0;
    const size_t probe_size = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap for each iteration
        std::memset(bitmap.data(), 0, bitmap_size * sizeof(uint64_t));

        // Mark enabled keys
        for (uint32_t key : enabled_keys) {
            bitmap[key >> 6] |= (1ULL << (key & 63));
        }

        uint64_t current_sum = 0;
        // Filter and sum
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t key = probe_keys[i];
            if (bitmap[key >> 6] & (1ULL << (key & 63))) {
                current_sum += values[i];
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}