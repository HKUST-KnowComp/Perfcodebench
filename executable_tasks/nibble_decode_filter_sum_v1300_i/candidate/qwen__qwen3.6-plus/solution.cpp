#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    
    for (size_t i = 0; i < n; ++i) {
        uint8_t b = data[i];
        uint8_t lo = b & 15;
        uint8_t hi = b >> 4;
        if (lo > threshold) sum += lo;
        if (hi > threshold) sum += hi;
    }
    return sum;
}
