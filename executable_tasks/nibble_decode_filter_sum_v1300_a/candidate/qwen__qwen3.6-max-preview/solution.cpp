#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    for (uint8_t b : packed) {
        uint8_t lo = b & 0x0F;
        uint8_t hi = b >> 4;
        if (lo > threshold) sum += lo;
        if (hi > threshold) sum += hi;
    }
    return sum;
}