#include "interface.h"
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
    // Early exit for no-op cases per valid input contract
    if (iters <= 0 || enabled_keys.empty() || probe_keys.empty() || values.empty()) {
        return 0;
    }

    // Create bounded bitmap lookup table (1 byte per key for fast access)
    std::vector<uint8_t> enabled_bitmap(key_space, 0);
    for (const uint32_t key : enabled_keys) {
        if (key < key_space) {
            enabled_bitmap[key] = 1;
        }
    }

    uint64_t final_result = 0;
    const size_t probe_count = probe_keys.size();
    const uint32_t* const probe_data = probe_keys.data();
    const uint32_t* const value_data = values.data();

    // Run all iterations efficiently with precomputed lookup
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < probe_count; ++i) {
            const uint32_t key = probe_data[i];
            if (key < key_space && enabled_bitmap[key]) {
                current_sum += static_cast<uint64_t>(value_data[i]);
            }
        }
        final_result = current_sum;
    }

    return final_result;
}