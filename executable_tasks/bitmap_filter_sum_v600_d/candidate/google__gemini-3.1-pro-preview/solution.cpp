#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    
    if (probe_keys.empty()) return 0;

    // Use a bitset for O(1) membership testing.
    // key_space is the upper bound for keys.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(bitmap_size, 0);

    for (uint32_t key : enabled_keys) {
        if (key < key_space) {
            bitmap[key >> 6] |= (1ULL << (key & 63));
        }
    }

    uint64_t total_sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_vals = values.data();
    const uint64_t* b_ptr = bitmap.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;

        // Unroll the loop to improve performance
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = p_keys[i];
            uint32_t k1 = p_keys[i + 1];
            uint32_t k2 = p_keys[i + 2];
            uint32_t k3 = p_keys[i + 3];

            if ((b_ptr[k0 >> 6] >> (k0 & 63)) & 1) current_sum += p_vals[i];
            if ((b_ptr[k1 >> 6] >> (k1 & 63)) & 1) current_sum += p_vals[i + 1];
            if ((b_ptr[k2 >> 6] >> (k2 & 63)) & 1) current_sum += p_vals[i + 2];
            if ((b_ptr[k3 >> 6] >> (k3 & 63)) & 1) current_sum += p_vals[i + 3];
        }

        for (; i < n; ++i) {
            uint32_t k = p_keys[i];
            if ((b_ptr[k >> 6] >> (k & 63)) & 1) {
                current_sum += p_vals[i];
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}