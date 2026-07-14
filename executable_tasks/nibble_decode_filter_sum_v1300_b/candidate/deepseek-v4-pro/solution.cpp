#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    for (std::size_t i = 0; i < packed.size(); ++i) {
        uint8_t byte = packed[i];
        uint32_t low = byte & 0x0F;
        if (low > threshold) sum += low;
        uint32_t high = byte >> 4;
        if (high > threshold) sum += high;
    }
    return sum;
}