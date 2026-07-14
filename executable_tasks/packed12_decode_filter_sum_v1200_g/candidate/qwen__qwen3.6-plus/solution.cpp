#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* p = packed.data();
    const uint8_t* end = p + packed.size();
    uint64_t sum = 0;
    uint32_t thr = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint8_t* cur = p;
        while (cur + 2 < end) {
            uint32_t a = cur[0] | ((cur[1] & 0x0F) << 8);
            uint32_t b = (cur[1] >> 4) | (cur[2] << 4);
            if (a > thr) sum += a;
            if (b > thr) sum += b;
            cur += 3;
        }
    }
    return sum;
}
