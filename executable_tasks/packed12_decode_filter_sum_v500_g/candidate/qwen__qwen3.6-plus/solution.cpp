#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* p = packed.data();
    const size_t n = packed.size();
    const uint32_t thr = threshold;
    uint64_t result = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;
        for (; i + 11 < n; i += 12) {
            uint32_t v1 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t v2 = (p[i+1] >> 4) | (p[i+2] << 4);
            uint32_t v3 = p[i+3] | ((p[i+4] & 0x0F) << 8);
            uint32_t v4 = (p[i+4] >> 4) | (p[i+5] << 4);
            uint32_t v5 = p[i+6] | ((p[i+7] & 0x0F) << 8);
            uint32_t v6 = (p[i+7] >> 4) | (p[i+8] << 4);
            uint32_t v7 = p[i+9] | ((p[i+10] & 0x0F) << 8);
            uint32_t v8 = (p[i+10] >> 4) | (p[i+11] << 4);

            if (v1 > thr) sum += v1;
            if (v2 > thr) sum += v2;
            if (v3 > thr) sum += v3;
            if (v4 > thr) sum += v4;
            if (v5 > thr) sum += v5;
            if (v6 > thr) sum += v6;
            if (v7 > thr) sum += v7;
            if (v8 > thr) sum += v8;
        }
        for (; i + 2 < n; i += 3) {
            uint32_t v1 = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t v2 = (p[i+1] >> 4) | (p[i+2] << 4);
            if (v1 > thr) sum += v1;
            if (v2 > thr) sum += v2;
        }
        result = sum;
    }
    return result;
}