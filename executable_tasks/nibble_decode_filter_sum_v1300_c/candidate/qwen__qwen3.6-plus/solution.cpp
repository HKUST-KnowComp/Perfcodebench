#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    const uint32_t t = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint8_t byte = data[i];
            uint32_t low = byte & 0x0F;
            uint32_t high = byte >> 4;
            if (low > t) current_sum += low;
            if (high > t) current_sum += high;
        }
        sum = current_sum;
    }
    return sum;
}