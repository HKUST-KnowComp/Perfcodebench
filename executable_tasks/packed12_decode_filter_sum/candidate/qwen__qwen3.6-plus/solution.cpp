#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const size_t n = packed.size();
    const uint32_t thr = threshold;
    const uint8_t* data = packed.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; i += 3) {
            uint32_t b0 = data[i];
            uint32_t b1 = data[i+1];
            uint32_t b2 = data[i+2];
            
            uint32_t v0 = b0 | ((b1 & 0x0F) << 8);
            uint32_t v1 = (b1 >> 4) | (b2 << 4);
            
            if (v0 > thr) current_sum += v0;
            if (v1 > thr) current_sum += v1;
        }
        sum = current_sum;
    }
    return sum;
}