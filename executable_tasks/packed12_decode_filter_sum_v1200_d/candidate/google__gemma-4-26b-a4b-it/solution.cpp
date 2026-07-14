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
        // Process in chunks of 3 bytes to extract two 12-bit values
        for (; i + 2 < n; i += 3) {
            // First 12-bit value: [byte0: 8 bits] [byte1: lower 4 bits]
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            
            // Second 12-bit value: [byte1: upper 4 bits] [byte2: 8 bits]
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4));

            if (a > thresh_u32) {
                current_iter_sum += a;
            }
            if (b > thresh_u32) {
                current_iter_sum += b;
            }
        }
        // The problem implies we return the sum of the LAST iteration based on the baseline logic
        // (sum = 0; ... sum = 0; for (v : decoded) sum += v; return sum;)
        total_sum = current_iter_sum;
    }

    return total_sum;
}