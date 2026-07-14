#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const uint8_t* end = data + packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        const uint8_t* p = data;
        while (p < end) {
            uint8_t b = *p++;
            uint8_t low = b & 15;
            uint8_t high = b >> 4;
            if (low > threshold) current_sum += low;
            if (high > threshold) current_sum += high;
        }
        sum = current_sum;
    }
    return sum;
}