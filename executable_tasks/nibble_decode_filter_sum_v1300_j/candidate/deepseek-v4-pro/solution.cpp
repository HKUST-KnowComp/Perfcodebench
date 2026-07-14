#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
    uint64_t sum = 0;
    for (auto byte : packed) {
        uint8_t lo = byte & 0x0F;
        uint8_t hi = byte >> 4;
        if (lo > threshold) sum += lo;
        if (hi > threshold) sum += hi;
    }
    return sum;
}