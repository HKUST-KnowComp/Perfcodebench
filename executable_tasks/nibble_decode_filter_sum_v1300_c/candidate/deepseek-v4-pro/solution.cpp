#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint32_t thres = threshold;
    for (size_t i = 0; i < n; ++i) {
        uint8_t byte = data[i];
        uint32_t lo = byte & 0x0F;
        uint32_t hi = byte >> 4;
        if (lo > thres) sum += lo;
        if (hi > thres) sum += hi;
    }
    return sum;
}
