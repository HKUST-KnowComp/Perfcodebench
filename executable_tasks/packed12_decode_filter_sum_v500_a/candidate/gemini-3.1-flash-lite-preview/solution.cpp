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
            // Packed 12-bit format: 3 bytes contain 2 values (12 bits each)
            // Byte 0: [7:0] of val1
            // Byte 1: [3:0] of val1, [7:4] of val2
            // Byte 2: [7:0] of val2
            const uint32_t v1 = static_cast<uint32_t>(data[i]) | 
                               (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            const uint32_t v2 = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                               (static_cast<uint32_t>(data[i + 2]) << 4);

            if (v1 > thresh) current_sum += v1;
            if (v2 > thresh) current_sum += v2;
        }
        total_sum = current_sum;
    }
    return total_sum;
}