#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    const uint32_t thr = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;
    for (const uint8_t byte : packed) {
        const uint32_t low = byte & 0x0Fu;
        if (low > thr) sum += low;
        const uint32_t high = byte >> 4u;
        if (high > thr) sum += high;
    }
    return sum;
}
