#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thresh_u32 = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t i = 0;
        
        // Process in 3-byte blocks to extract two 12-bit values
        // Each block of 3 bytes contains 2 values: 
        // val1: [byte0: 8 bits][byte1 low 4 bits: 4 bits]
        // val2: [byte1 high 4 bits: 4 bits][byte2: 8 bits]
        for (; i + 2 < n; i += 3) {
            uint32_t b0 = data[i];
            uint32_t b1 = data[i + 1];
            uint32_t b2 = data[i + 2];

            uint32_t v1 = b0 | ((b1 & 0x0F) << 8);
            uint32_t v2 = (b1 >> 4) | (b2 << 4);

            if (v1 > thresh_u32) {
                current_iter_sum += v1;
            }
            if (v2 > thresh_u32) {
                current_iter_sum += v2;
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}