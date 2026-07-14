#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* p = packed.data();
    const std::size_t n = packed.size();
    const uint32_t th = threshold;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        std::size_t i = 0;
        for (; i + 5 < n; i += 6) {
            uint32_t v0 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t v1 = (p[i+1] >> 4) | (p[i+2] << 4);
            uint32_t v2 = p[i+3] | ((p[i+4] & 0x0F) << 8);
            uint32_t v3 = (p[i+4] >> 4) | (p[i+5] << 4);
            
            if (v0 > th) s += v0;
            if (v1 > th) s += v1;
            if (v2 > th) s += v2;
            if (v3 > th) s += v3;
        }
        for (; i < n; i += 3) {
            uint32_t v0 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t v1 = (p[i+1] >> 4) | (p[i+2] << 4);
            if (v0 > th) s += v0;
            if (v1 > th) s += v1;
        }
        sum = s;
    }
    return sum;
}
