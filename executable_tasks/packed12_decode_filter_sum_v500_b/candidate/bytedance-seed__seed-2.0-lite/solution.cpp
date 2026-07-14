#include "interface.h"
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;
    const std::size_t packed_size = packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < packed_size; i += 3) {
            // Unpack two 12-bit values from 3 input bytes, matching original decoding logic
            const uint32_t a = static_cast<uint32_t>(packed[i]) |
                               (static_cast<uint32_t>(packed[i + 1] & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(packed[i + 1] >> 4)) |
                               (static_cast<uint32_t>(packed[i + 2]) << 4);
            
            // Accumulate values exceeding threshold directly, no intermediate storage
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
        }
    }
    return sum;
}