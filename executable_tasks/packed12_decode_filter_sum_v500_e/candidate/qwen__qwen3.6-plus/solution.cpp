#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const size_t n = packed.size();
    const size_t limit = (n / 3) * 3;
    const uint8_t* p = packed.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < limit; i += 3) {
            uint32_t w = p[i] | (static_cast<uint32_t>(p[i+1]) << 8) | (static_cast<uint32_t>(p[i+2]) << 16);
            uint32_t a = w & 0xFFF;
            uint32_t b = (w >> 12) & 0xFFF;
            if (a > threshold) sum += a;
            if (b > threshold) sum += b;
        }
    }
    return sum;
}
