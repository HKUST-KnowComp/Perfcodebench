#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    uint64_t total_sum = 0;
    const uint32_t u_threshold = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; i += 3) {
            // Decode first 12-bit value: 
            // Byte 0 (8 bits) | (Byte 1 lower 4 bits << 8)
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            
            // Decode second 12-bit value:
            // (Byte 1 upper 4 bits) | (Byte 2 (8 bits) << 4)
            uint32_t b = static_cast<uint32_t>(data[i + 1] >> 4) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4);

            if (a > u_threshold) current_sum += a;
            if (b > u_threshold) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}