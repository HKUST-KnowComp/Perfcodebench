#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint8_t thresh = threshold;
    for (size_t i = 0; i < n; ++i) {
        uint8_t b = data[i];
        uint8_t low = b & 0x0F;
        uint8_t high = b >> 4;
        if (low > thresh) sum += low;
        if (high > thresh) sum += high;
    }
    return sum;
}
