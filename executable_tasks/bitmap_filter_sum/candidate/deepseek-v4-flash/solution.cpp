#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    // Edge cases: no keys or empty probe -> sum is always 0
    if (key_space == 0 || probe_keys.empty()) {
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
        }
        return 0;
    }

    // Build the bitmap once
    size_t bitset_size = (key_space + 63) / 64;
    std::vector<uint64_t> bitmap(bitset_size, 0);
    for (uint32_t key : enabled_keys) {
        bitmap[key >> 6] |= (1ULL << (key & 63));
    }

    uint64_t sum = 0;
    const size_t n = probe_keys.size();
    const uint32_t* pk = probe_keys.data();
    const uint32_t* vals = values.data();
    const uint64_t* map = bitmap.data();

    // Repeat the filter iters times (deterministic data -> same sum each time)
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t k = pk[i];
            if (map[k >> 6] & (1ULL << (k & 63))) {
                sum += vals[i];
            }
        }
    }
    return sum;
}
