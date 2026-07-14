#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint8_t* data = packed.data();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i + 2 < n; i += 3) {
            // Decode two 12-bit integers from 3 bytes
            // Byte layout: [b0, b1_low, b1_high, b2]
            // val1 = b0 | ((b1 & 0x0F) << 8)
            // val2 = (b1 >> 4) | (b2 << 4)
            const uint32_t b0 = data[i];
            const uint32_t b1 = data[i + 1];
            const uint32_t b2 = data[i + 2];

            const uint32_t v1 = b0 | ((b1 & 0x0Fu) << 8u);
            const uint32_t v2 = (b1 >> 4u) | (b2 << 4u);

            if (v1 > thresh) current_sum += v1;
            if (v2 > thresh) current_sum += v2;
        }
        total_sum = current_sum;
    }
    return total_sum;
}