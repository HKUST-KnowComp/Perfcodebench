#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    size_t limit = (n / 3) * 3;
    uint64_t sum = 0;
    uint32_t thr = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        const uint8_t* p = data;
        const uint8_t* end = data + limit;
        while (p < end) {
            uint32_t a = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0F) << 8);
            uint32_t b = (static_cast<uint32_t>(p[1]) >> 4) | (static_cast<uint32_t>(p[2]) << 4);
            if (a > thr) iter_sum += a;
            if (b > thr) iter_sum += b;
            p += 3;
        }
        sum = iter_sum;
    }
    return sum;
}
