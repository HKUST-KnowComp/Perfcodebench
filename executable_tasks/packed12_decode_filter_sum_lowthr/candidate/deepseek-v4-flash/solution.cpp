#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint32_t thr = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; i += 3) {
            uint32_t byte0 = data[i];
            uint32_t byte1 = data[i + 1];
            uint32_t byte2 = data[i + 2];
            uint32_t a = byte0 | ((byte1 & 0x0F) << 8);
            uint32_t b = (byte1 >> 4) | (byte2 << 4);
            if (a > thr) iter_sum += a;
            if (b > thr) iter_sum += b;
        }
        sum = iter_sum;
    }
    return sum;
}
