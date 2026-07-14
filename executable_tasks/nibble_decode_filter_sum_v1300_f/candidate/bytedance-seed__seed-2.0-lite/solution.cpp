#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const std::size_t packed_len = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const uint8_t* __restrict__ data = packed.data();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < packed_len; ++i) {
            const uint8_t b = data[i];
            // Process low nibble first to match baseline order
            const uint32_t low = static_cast<uint32_t>(b & 0x0F);
            if (low > thresh) sum += low;
            // Process high nibble
            const uint32_t high = static_cast<uint32_t>(b >> 4);
            if (high > thresh) sum += high;
        }
        final_sum = sum;
    }
    return final_sum;
}