#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    // Prebuild a byte-vector bitmap of size key_space for fast random access
    std::vector<uint8_t> bitmap(key_space, 0);
    for (auto key : enabled_keys) {
        bitmap[key] = 1;
    }

    const size_t n = probe_keys.size();
    const uint32_t* probe = probe_keys.data();
    const uint32_t* val = values.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (bitmap[probe[i]]) {
                local_sum += static_cast<uint64_t>(val[i]);
            }
        }
        sum = local_sum;
    }
    return sum;
}
