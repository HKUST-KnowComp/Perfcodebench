#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t packed_len = packed.size();
    const uint8_t* const packed_data = packed.data();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < packed_len; ++i) {
            const uint8_t byte = packed_data[i];
            // Process low nibble first to match original order
            const uint32_t low_nib = static_cast<uint32_t>(byte & 0xFU);
            if (low_nib > thresh) {
                sum += low_nib;
            }
            // Process high nibble to match original order
            const uint32_t high_nib = static_cast<uint32_t>(byte >> 4U);
            if (high_nib > thresh) {
                sum += high_nib;
            }
        }
    }

    return sum;
}