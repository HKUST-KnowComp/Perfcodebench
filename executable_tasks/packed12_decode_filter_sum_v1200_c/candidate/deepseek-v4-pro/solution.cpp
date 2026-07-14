#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
    uint64_t sum = 0;
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t size = packed.size();
    for (std::size_t i = 0; i < size; i += 3) {
        const uint8_t b0 = packed[i];
        const uint8_t b1 = packed[i + 1];
        const uint8_t b2 = packed[i + 2];
        const uint32_t a = static_cast<uint32_t>(b0) | ((static_cast<uint32_t>(b1) & 0x0Fu) << 8);
        const uint32_t b = (static_cast<uint32_t>(b1) >> 4) | (static_cast<uint32_t>(b2) << 4);
        sum += (a > thresh) ? a : 0;
        sum += (b > thresh) ? b : 0;
    }
    return sum;
}
