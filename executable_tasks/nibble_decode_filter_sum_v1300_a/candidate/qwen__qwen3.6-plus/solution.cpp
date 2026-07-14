#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t l = b & 15;
            uint8_t h = b >> 4;
            if (l > threshold) current_sum += l;
            if (h > threshold) current_sum += h;
        }
        sum = current_sum;
    }
    return sum;
}