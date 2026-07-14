#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* p = packed.data();
    const std::size_t n = packed.size();
    const uint32_t thr = threshold;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (std::size_t i = 0; i + 2 < n; i += 3) {
            uint32_t a = p[i] | ((p[i + 1] & 0x0F) << 8);
            uint32_t b = (p[i + 1] >> 4) | (p[i + 2] << 4);
            if (a > thr) local += a;
            if (b > thr) local += b;
        }
        sum = local;
    }
    return sum;
}