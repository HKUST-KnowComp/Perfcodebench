#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thr = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;
        // Process 4 triplets (12 bytes, 8 values) per iteration
        for (; i + 11 < n; i += 12) {
            uint8_t m0 = data[i+1];
            uint8_t m1 = data[i+4];
            uint8_t m2 = data[i+7];
            uint8_t m3 = data[i+10];
            
            uint32_t a0 = data[i] | ((m0 & 0x0F) << 8);
            uint32_t b0 = (m0 >> 4) | (data[i+2] << 4);
            uint32_t a1 = data[i+3] | ((m1 & 0x0F) << 8);
            uint32_t b1 = (m1 >> 4) | (data[i+5] << 4);
            uint32_t a2 = data[i+6] | ((m2 & 0x0F) << 8);
            uint32_t b2 = (m2 >> 4) | (data[i+8] << 4);
            uint32_t a3 = data[i+9] | ((m3 & 0x0F) << 8);
            uint32_t b3 = (m3 >> 4) | (data[i+11] << 4);

            if (a0 > thr) sum += a0;
            if (b0 > thr) sum += b0;
            if (a1 > thr) sum += a1;
            if (b1 > thr) sum += b1;
            if (a2 > thr) sum += a2;
            if (b2 > thr) sum += b2;
            if (a3 > thr) sum += a3;
            if (b3 > thr) sum += b3;
        }
        // Handle remaining bytes
        for (; i + 2 < n; i += 3) {
            uint8_t m = data[i+1];
            uint32_t a = data[i] | ((m & 0x0F) << 8);
            uint32_t b = (m >> 4) | (data[i+2] << 4);
            if (a > thr) sum += a;
            if (b > thr) sum += b;
        }
    }
    return sum;
}