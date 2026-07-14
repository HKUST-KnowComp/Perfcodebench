#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
    const std::size_t n = packed.size();
    uint64_t sum = 0;
    const uint8_t* p = packed.data();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (std::size_t i = 0; i < n; i += 3) {
        uint32_t byte0 = p[i];
        uint32_t byte1 = p[i+1];
        uint32_t byte2 = p[i+2];

        uint32_t a = byte0 | ((byte1 & 0x0F) << 8);
        uint32_t b = (byte1 >> 4) | (byte2 << 4);

        sum += a * (a > thresh);
        sum += b * (b > thresh);
    }

    return sum;
}