#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    for (const auto byte : packed) {
        uint8_t low  = byte & 0x0Fu;
        uint8_t high = byte >> 4u;
        if (low > threshold)  sum += low;
        if (high > threshold) sum += high;
    }
    return sum;
}
