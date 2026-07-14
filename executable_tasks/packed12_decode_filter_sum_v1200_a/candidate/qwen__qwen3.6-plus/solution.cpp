#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint64_t sum = 0;
    uint32_t thr = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i + 2 < n; i += 3) {
            uint32_t mid = data[i+1];
            uint32_t a = data[i] | ((mid & 0x0F) << 8);
            uint32_t b = (mid >> 4) | (data[i+2] << 4);
            if (a > thr) sum += a;
            if (b > thr) sum += b;
        }
    }
    return sum;
}
