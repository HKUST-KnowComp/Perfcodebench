#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t len = packed.size();
    const size_t limit = len - (len % 3);
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < limit; i += 3) {
            uint32_t a = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i+1] & 0x0F) << 8);
            uint32_t b = static_cast<uint32_t>(data[i+1] >> 4) | (static_cast<uint32_t>(data[i+2]) << 4);
            if (a > threshold) current_sum += a;
            if (b > threshold) current_sum += b;
        }
        sum = current_sum;
    }
    return sum;
}