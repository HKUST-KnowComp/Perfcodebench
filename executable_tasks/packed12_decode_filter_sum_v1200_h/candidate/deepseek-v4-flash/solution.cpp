#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    std::size_t n = packed.size();
    uint32_t thresh = threshold;
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint8_t* p = data;
        const uint8_t* end = data + n;
        while (p < end) {
            uint32_t a = p[0] | ((uint32_t)(p[1] & 0x0Fu) << 8u);
            uint32_t b = ((uint32_t)(p[1] >> 4u)) | ((uint32_t)p[2] << 4u);
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
            p += 3;
        }
    }
    return sum;
}
