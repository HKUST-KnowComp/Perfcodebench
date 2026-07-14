#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* p = packed.data();
    const size_t n = packed.size();
    const size_t limit = (n / 3) * 3;
    const uint32_t thr = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < limit; i += 3) {
            uint32_t a = p[i] | ((p[i+1] & 0x0F) << 8);
            uint32_t b = (p[i+1] >> 4) | (p[i+2] << 4);
            if (a > thr) sum += a;
            if (b > thr) sum += b;
        }
    }
    return sum;
}