#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const size_t n = packed.size();
    const uint8_t* p = packed.data();
    const uint32_t thr = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Unroll by 4 groups (12 bytes) for better ILP and reduced loop overhead
        for (; i + 12 <= n; i += 12) {
            uint32_t a0 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b0 = (p[i+1] >> 4) | (p[i+2] << 4);
            uint32_t a1 = p[i+3] | ((p[i+4] & 0x0F) << 8);
            uint32_t b1 = (p[i+4] >> 4) | (p[i+5] << 4);
            uint32_t a2 = p[i+6] | ((p[i+7] & 0x0F) << 8);
            uint32_t b2 = (p[i+7] >> 4) | (p[i+8] << 4);
            uint32_t a3 = p[i+9] | ((p[i+10] & 0x0F) << 8);
            uint32_t b3 = (p[i+10] >> 4) | (p[i+11] << 4);
            
            if (a0 > thr) current_sum += a0;
            if (b0 > thr) current_sum += b0;
            if (a1 > thr) current_sum += a1;
            if (b1 > thr) current_sum += b1;
            if (a2 > thr) current_sum += a2;
            if (b2 > thr) current_sum += b2;
            if (a3 > thr) current_sum += a3;
            if (b3 > thr) current_sum += b3;
        }
        
        for (; i + 2 < n; i += 3) {
            uint32_t a = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b = (p[i+1] >> 4) | (p[i+2] << 4);
            if (a > thr) current_sum += a;
            if (b > thr) current_sum += b;
        }
        sum = current_sum;
    }
    return sum;
}