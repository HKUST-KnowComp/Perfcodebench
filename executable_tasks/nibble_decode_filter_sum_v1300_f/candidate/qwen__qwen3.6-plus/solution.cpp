#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo = b & 0xF;
            uint8_t hi = b >> 4;
            if (lo > threshold) current_sum += lo;
            if (hi > threshold) current_sum += hi;
        }
        sum = current_sum;
    }
    return sum;
}