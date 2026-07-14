#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t packed_len = packed.size();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < packed_len; ++i) {
            const uint8_t byte = packed[i];
            // Process low 4-bit nibble
            uint32_t v = static_cast<uint32_t>(byte & 0x0Fu);
            if (v > thresh) sum += v;
            // Process high 4-bit nibble
            v = static_cast<uint32_t>(byte >> 4u);
            if (v > thresh) sum += v;
        }
        final_sum = sum;
    }
    return final_sum;
}