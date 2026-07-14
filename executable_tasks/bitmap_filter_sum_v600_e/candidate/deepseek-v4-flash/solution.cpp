#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    const size_t n_enabled = enabled_keys.size();
    const size_t n_probe = probe_keys.size();
    const uint32_t* enabled_data = enabled_keys.data();
    const uint32_t* probe_data = probe_keys.data();
    const uint32_t* values_data = values.data();

    std::vector<uint8_t> bitmap(key_space, 0);
    uint8_t* bitmap_data = bitmap.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // set enabled keys in bitmap
        for (size_t i = 0; i < n_enabled; ++i) {
            bitmap_data[enabled_data[i]] = 1;
        }
        // compute sum of values for matching probe keys
        sum = 0;
        for (size_t i = 0; i < n_probe; ++i) {
            if (bitmap_data[probe_data[i]]) {
                sum += values_data[i];
            }
        }
        // clear enabled keys
        for (size_t i = 0; i < n_enabled; ++i) {
            bitmap_data[enabled_data[i]] = 0;
        }
    }
    return sum;
}
