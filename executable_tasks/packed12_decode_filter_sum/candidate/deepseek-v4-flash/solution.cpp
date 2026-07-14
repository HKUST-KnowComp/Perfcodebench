#include "interface.h"
#include <cstdint>
#include <cstddef>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    std::size_t triplets = packed.size() / 3;
    uint64_t sum = 0;
    uint32_t thresh = threshold;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        uint64_t iter_sum = 0;
        for (std::size_t t = 0; t < triplets; ++t) {
            uint32_t b0 = *p++;
            uint32_t b1 = *p++;
            uint32_t b2 = *p++;
            
            uint32_t a = b0 | ((b1 & 0x0F) << 8);
            uint32_t b = (b1 >> 4) | (b2 << 4);
            
            if (a > thresh) iter_sum += a;
            if (b > thresh) iter_sum += b;
        }
        sum = iter_sum;
    }
    return sum;
}