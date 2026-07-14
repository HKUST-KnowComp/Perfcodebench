#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (packed.empty() || iters <= 0) return 0;

    uint64_t last_sum = 0;
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh_u32 = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i + 2 < size; i += 3) {
            // Decode first 12-bit value
            // a = packed[i] | (packed[i+1] & 0x0F << 8)
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         ((static_cast<uint32_t>(data[i + 1]) & 0x0F) << 8);
            
            if (a > thresh_u32) {
                current_sum += a;
            }

            // Decode second 12-bit value
            // b = (packed[i+1] >> 4) | (packed[i+2] << 4)
            uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4);
            
            if (b > thresh_u32) {
                current_sum += b;
            }
        }
        last_sum = current_sum;
    }

    return last_sum;
}