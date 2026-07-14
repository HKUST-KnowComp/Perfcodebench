#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh32 = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint8_t* p = data;
        const uint8_t* end = data + size;
        while (p < end) {
            uint8_t b0 = p[0];
            uint8_t b1 = p[1];
            uint8_t b2 = p[2];
            uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
            uint32_t b = (b1 >> 4) | (b2 << 4);
            sum += (a > thresh32) ? a : 0;
            sum += (b > thresh32) ? b : 0;
            p += 3;
        }
    }
    return sum;
}