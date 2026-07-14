#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i + 2 < n; i += 3) {
            // Packed 12-bit format: 3 bytes contain 2 values (12 bits each)
            // Byte 0: [7:0] of val1
            // Byte 1: [3:0] of val1, [7:4] of val2
            // Byte 2: [7:0] of val2
            const uint32_t val1 = static_cast<uint32_t>(packed[i]) | 
                                 ((static_cast<uint32_t>(packed[i + 1]) & 0x0Fu) << 8);
            const uint32_t val2 = (static_cast<uint32_t>(packed[i + 1]) >> 4) | 
                                 (static_cast<uint32_t>(packed[i + 2]) << 4);

            if (val1 > thresh) current_sum += val1;
            if (val2 > thresh) current_sum += val2;
        }
        total_sum = current_sum;
    }
    return total_sum;
}