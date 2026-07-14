#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const uint8_t* end = data + packed.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (const uint8_t* p = data; p != end; ++p) {
            uint8_t b = *p;
            uint8_t lo = b & 0xF;
            uint8_t hi = b >> 4;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
    return sum;
}