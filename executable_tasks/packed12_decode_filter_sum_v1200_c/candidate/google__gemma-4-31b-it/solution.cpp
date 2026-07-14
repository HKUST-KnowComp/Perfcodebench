#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint8_t* data = packed.data();
    uint64_t total_sum = 0;
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; i += 3) {
            // Decode first 12-bit value: 
            // Byte 0 (8 bits) | (Byte 1 & 0x0F) << 8
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1]) << 8);
            // Masking the overflow from Byte 1 is implicit if we use 0x0F, 
            // but the original logic was: (packed[i+1] & 0x0f) << 8
            a = (static_cast<uint32_t>(data[i])) | 
                 ((static_cast<uint32_t>(data[i + 1]) & 0x0Fu) << 8u);
            
            // Decode second 12-bit value:
            // (Byte 1 >> 4) | (Byte 2 << 4)
            uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4u) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4u);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}