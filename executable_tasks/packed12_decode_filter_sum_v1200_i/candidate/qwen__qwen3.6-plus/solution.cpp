#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* p = packed.data();
    const size_t n = packed.size();
    const size_t n_aligned = n - (n % 12);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        size_t i = 0;
        for (; i < n_aligned; i += 12) {
            uint32_t a0 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b0 = (p[i+1] >> 4) | (p[i+2] << 4);
            uint32_t a1 = p[i+3] | ((p[i+4] & 0x0F) << 8);
            uint32_t b1 = (p[i+4] >> 4) | (p[i+5] << 4);
            uint32_t a2 = p[i+6] | ((p[i+7] & 0x0F) << 8);
            uint32_t b2 = (p[i+7] >> 4) | (p[i+8] << 4);
            uint32_t a3 = p[i+9] | ((p[i+10] & 0x0F) << 8);
            uint32_t b3 = (p[i+10] >> 4) | (p[i+11] << 4);

            if (a0 > threshold) s += a0;
            if (b0 > threshold) s += b0;
            if (a1 > threshold) s += a1;
            if (b1 > threshold) s += b1;
            if (a2 > threshold) s += a2;
            if (b2 > threshold) s += b2;
            if (a3 > threshold) s += a3;
            if (b3 > threshold) s += b3;
        }
        for (; i < n; i += 3) {
            uint32_t a = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b = (p[i+1] >> 4) | (p[i+2] << 4);
            if (a > threshold) s += a;
            if (b > threshold) s += b;
        }
        sum = s;
    }
    return sum;
}
