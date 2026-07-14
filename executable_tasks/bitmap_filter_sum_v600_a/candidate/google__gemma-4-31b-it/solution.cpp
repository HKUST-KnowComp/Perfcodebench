#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    // Use a byte-array as a bitmap for O(1) access. 
    // key_space is the bound for the keys.
    std::vector<uint8_t> bitmap(key_space + 1, 0);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Fast clear of the bitmap
        std::memset(bitmap.data(), 0, bitmap.size());

        // Mark enabled keys
        for (uint32_t key : enabled_keys) {
            if (key <= key_space) {
                bitmap[key] = 1;
            }
        }

        uint64_t current_sum = 0;
        const size_t n = probe_keys.size();
        const uint32_t* p_keys = probe_keys.data();
        const uint32_t* p_vals = values.data();

        // Filter and aggregate
        for (size_t i = 0; i < n; ++i) {
            uint32_t key = p_keys[i];
            if (key <= key_space && bitmap[key]) {
                current_sum += p_vals[i];
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}