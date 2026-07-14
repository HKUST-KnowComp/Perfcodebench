#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thresh = threshold;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        size_t i = 0;
        for (; i + 12 <= n; i += 12) {
            uint32_t a0 = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t b0 = (data[i+1] >> 4) | (data[i+2] << 4);
            uint32_t a1 = data[i+3] | ((data[i+4] & 0x0F) << 8);
            uint32_t b1 = (data[i+4] >> 4) | (data[i+5] << 4);
            uint32_t a2 = data[i+6] | ((data[i+7] & 0x0F) << 8);
            uint32_t b2 = (data[i+7] >> 4) | (data[i+8] << 4);
            uint32_t a3 = data[i+9] | ((data[i+10] & 0x0F) << 8);
            uint32_t b3 = (data[i+10] >> 4) | (data[i+11] << 4);

            if (a0 > thresh) s0 += a0;
            if (b0 > thresh) s0 += b0;
            if (a1 > thresh) s1 += a1;
            if (b1 > thresh) s1 += b1;
            if (a2 > thresh) s2 += a2;
            if (b2 > thresh) s2 += b2;
            if (a3 > thresh) s3 += a3;
            if (b3 > thresh) s3 += b3;
        }
        for (; i < n; i += 3) {
            uint32_t a = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t b = (data[i+1] >> 4) | (data[i+2] << 4);
            if (a > thresh) s0 += a;
            if (b > thresh) s0 += b;
        }
        sum = s0 + s1 + s2 + s3;
    }
    return sum;
}