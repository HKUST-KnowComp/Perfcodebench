#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
    uint64_t table[16];
    uint64_t th = threshold;
    for (int i = 0; i < 16; ++i) {
        table[i] = (static_cast<uint64_t>(i) > th) ? i : 0;
    }
    uint64_t sum = 0;
    for (uint8_t b : packed) {
        sum += table[b & 0x0F];
        sum += table[b >> 4];
    }
    return sum;
}