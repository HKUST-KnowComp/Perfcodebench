#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint32_t thresh_u32 = static_cast<uint32_t>(threshold);
    const uint8_t* data = packed.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i + 2 < n; i += 3) {
            // Extract first 12-bit value: [byte0: 8 bits] [byte1: low 4 bits]
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            
            // Extract second 12-bit value: [byte1: high 4 bits] [byte2: 8 bits]
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4);

            if (a > thresh_u32) {
                current_iter_sum += a;
            }
            if (b > thresh_u32) {
                current_iter_sum += b;
            }
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}