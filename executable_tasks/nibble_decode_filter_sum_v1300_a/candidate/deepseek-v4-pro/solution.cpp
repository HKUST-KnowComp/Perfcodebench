#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
    uint64_t sum = 0;
    uint32_t threshold32 = static_cast<uint32_t>(threshold);
    for (std::size_t i = 0; i < packed.size(); ++i) {
        uint8_t byte = packed[i];
        uint32_t low = byte & 0x0Fu;
        uint32_t high = byte >> 4u;
        if (low > threshold32) sum += low;
        if (high > threshold32) sum += high;
    }
    return sum;
}
