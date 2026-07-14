#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; i += 3) {
            uint32_t a = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t b = (data[i+1] >> 4) | (data[i+2] << 4);
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
        }
    }
    return sum;
}