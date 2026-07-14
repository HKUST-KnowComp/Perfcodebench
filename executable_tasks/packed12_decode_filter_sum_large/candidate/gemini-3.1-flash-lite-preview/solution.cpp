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
        for (size_t i = 0; i + 2 < n; i += 3) {
            // Decode two 12-bit integers from 3 bytes
            // Byte layout: [B0, B1, B2]
            // Int 1: (B0) | ((B1 & 0x0F) << 8)
            // Int 2: (B1 >> 4) | (B2 << 4)
            const uint32_t v1 = static_cast<uint32_t>(data[i]) | 
                               (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            const uint32_t v2 = (static_cast<uint32_t>(data[i + 1]) >> 4) | 
                               (static_cast<uint32_t>(data[i + 2]) << 4);

            if (v1 > thresh) {
                current_sum += v1;
            }
            if (v2 > thresh) {
                current_sum += v2;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}