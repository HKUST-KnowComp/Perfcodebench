#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const uint8_t* data = packed.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; i += 3) {
            // Decode two 12-bit integers from 3 bytes
            // Byte layout: [B0, B1, B2]
            // Int1: (B0) | ((B1 & 0x0F) << 8)
            // Int2: (B1 >> 4) | (B2 << 4)
            
            const uint32_t b0 = data[i];
            const uint32_t b1 = data[i + 1];
            const uint32_t b2 = data[i + 2];

            const uint32_t val1 = b0 | ((b1 & 0x0Fu) << 8u);
            const uint32_t val2 = (b1 >> 4u) | (b2 << 4u);

            if (val1 > thresh) {
                current_sum += val1;
            }
            if (val2 > thresh) {
                current_sum += val2;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}