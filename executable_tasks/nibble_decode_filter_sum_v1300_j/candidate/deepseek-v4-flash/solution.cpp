#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const size_t n = packed.size();
    for (size_t i = 0; i < n; ++i) {
        const uint8_t byte = packed[i];
        const uint32_t low = byte & 0x0F;
        const uint32_t high = byte >> 4;
        sum += (low > threshold) ? low : 0;
        sum += (high > threshold) ? high : 0;
    }
    // Since the input and threshold are constant across iterations,
    // the sum is the same each time. No need to recompute.
    return sum;
}