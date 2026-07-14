#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    (void)iters;
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    
    for (size_t i = 0; i < n; ++i) {
        uint8_t b = data[i];
        uint8_t low = b & 0xF;
        uint8_t high = b >> 4;
        if (low > threshold) sum += low;
        if (high > threshold) sum += high;
    }
    return sum;
}