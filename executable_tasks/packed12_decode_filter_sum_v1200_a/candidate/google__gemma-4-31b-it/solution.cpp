#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; i += 3) {
            // Decode two 12-bit values from 3 bytes
            // Byte 0: A(low 8), Byte 1: A(high 4) | B(low 4), Byte 2: B(high 8)
            const uint8_t b0 = data[i];
            const uint8_t b1 = data[i + 1];
            const uint8_t b2 = data[i + 2];

            const uint32_t a = static_cast<uint32_t>(b0) | (static_cast<uint32_t>(b1 & 0x0F) << 8);
            const uint32_t b = static_cast<uint32_t>(b1 >> 4) | (static_cast<uint32_t>(b2) << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}