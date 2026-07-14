#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    std::size_t n = packed.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t b = data[i];
            uint32_t lo = b & 15u;
            uint32_t hi = b >> 4u;
            s += (lo > threshold) ? lo : 0;
            s += (hi > threshold) ? hi : 0;
        }
        sum = s;
    }
    return sum;
}