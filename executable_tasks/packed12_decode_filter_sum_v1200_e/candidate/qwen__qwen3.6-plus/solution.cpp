#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* p = packed.data();
    const size_t n = packed.size();
    const uint32_t th = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Process in chunks of 24 bytes (8 groups of 3 bytes -> 16 values)
        for (; i + 24 <= n; i += 24) {
            uint32_t v0 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t v1 = (p[i+1] >> 4) | (p[i+2] << 4);
            uint32_t v2 = p[i+3] | ((p[i+4] & 0x0F) << 8);
            uint32_t v3 = (p[i+4] >> 4) | (p[i+5] << 4);
            uint32_t v4 = p[i+6] | ((p[i+7] & 0x0F) << 8);
            uint32_t v5 = (p[i+7] >> 4) | (p[i+8] << 4);
            uint32_t v6 = p[i+9] | ((p[i+10] & 0x0F) << 8);
            uint32_t v7 = (p[i+10] >> 4) | (p[i+11] << 4);
            uint32_t v8 = p[i+12] | ((p[i+13] & 0x0F) << 8);
            uint32_t v9 = (p[i+13] >> 4) | (p[i+14] << 4);
            uint32_t v10 = p[i+15] | ((p[i+16] & 0x0F) << 8);
            uint32_t v11 = (p[i+16] >> 4) | (p[i+17] << 4);
            uint32_t v12 = p[i+18] | ((p[i+19] & 0x0F) << 8);
            uint32_t v13 = (p[i+19] >> 4) | (p[i+20] << 4);
            uint32_t v14 = p[i+21] | ((p[i+22] & 0x0F) << 8);
            uint32_t v15 = (p[i+22] >> 4) | (p[i+23] << 4);

            if (v0 > th) current_sum += v0;
            if (v1 > th) current_sum += v1;
            if (v2 > th) current_sum += v2;
            if (v3 > th) current_sum += v3;
            if (v4 > th) current_sum += v4;
            if (v5 > th) current_sum += v5;
            if (v6 > th) current_sum += v6;
            if (v7 > th) current_sum += v7;
            if (v8 > th) current_sum += v8;
            if (v9 > th) current_sum += v9;
            if (v10 > th) current_sum += v10;
            if (v11 > th) current_sum += v11;
            if (v12 > th) current_sum += v12;
            if (v13 > th) current_sum += v13;
            if (v14 > th) current_sum += v14;
            if (v15 > th) current_sum += v15;
        }
        for (; i < n; i += 3) {
            uint32_t a = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b = (p[i+1] >> 4) | (p[i+2] << 4);
            if (a > th) current_sum += a;
            if (b > th) current_sum += b;
        }
        sum = current_sum;
    }
    return sum;
}