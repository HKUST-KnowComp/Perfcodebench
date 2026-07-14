#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t len = packed.size();
    const uint32_t thresh = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < len; i += 3) {
            uint32_t a = static_cast<uint32_t>(data[i]) |
                         ((static_cast<uint32_t>(data[i + 1]) & 0x0Fu) << 8u);
            uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4u) |
                         (static_cast<uint32_t>(data[i + 2]) << 4u);
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
        }
    }
    return sum;
}
