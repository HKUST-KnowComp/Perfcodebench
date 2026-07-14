#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const size_t limit = (n / 3) * 3;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < limit; i += 3) {
            uint32_t chunk = data[i] | (data[i+1] << 8) | (data[i+2] << 16);
            uint32_t a = chunk & 0x0FFF;
            uint32_t b = (chunk >> 12) & 0x0FFF;
            if (a > threshold) current_sum += a;
            if (b > threshold) current_sum += b;
        }
        sum = current_sum;
    }
    return sum;
}