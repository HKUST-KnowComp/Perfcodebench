#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    if (threshold >= 15) return 0; // Nibbles are 0-15, so nothing can be > 15

    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const uint8_t* end = data + packed.size();

    for (const uint8_t* p = data; p < end; ++p) {
        uint8_t low = *p & 0x0F;
        uint8_t high = *p >> 4;

        if (low > threshold) sum += low;
        if (high > threshold) sum += high;
    }

    return sum;
}