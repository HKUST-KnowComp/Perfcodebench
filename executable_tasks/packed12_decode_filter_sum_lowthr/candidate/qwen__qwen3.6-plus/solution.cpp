#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const size_t limit = n - (n % 3);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        size_t i = 0;
        for (; i + 12 <= limit; i += 12) {
            uint32_t v0 = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t v1 = (data[i+1] >> 4) | (data[i+2] << 4);
            uint32_t v2 = data[i+3] | ((data[i+4] & 0x0F) << 8);
            uint32_t v3 = (data[i+4] >> 4) | (data[i+5] << 4);
            uint32_t v4 = data[i+6] | ((data[i+7] & 0x0F) << 8);
            uint32_t v5 = (data[i+7] >> 4) | (data[i+8] << 4);
            uint32_t v6 = data[i+9] | ((data[i+10] & 0x0F) << 8);
            uint32_t v7 = (data[i+10] >> 4) | (data[i+11] << 4);

            if (v0 > threshold) s += v0;
            if (v1 > threshold) s += v1;
            if (v2 > threshold) s += v2;
            if (v3 > threshold) s += v3;
            if (v4 > threshold) s += v4;
            if (v5 > threshold) s += v5;
            if (v6 > threshold) s += v6;
            if (v7 > threshold) s += v7;
        }
        for (; i < limit; i += 3) {
            uint32_t v0 = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t v1 = (data[i+1] >> 4) | (data[i+2] << 4);
            if (v0 > threshold) s += v0;
            if (v1 > threshold) s += v1;
        }
        sum = s;
    }
    return sum;
}