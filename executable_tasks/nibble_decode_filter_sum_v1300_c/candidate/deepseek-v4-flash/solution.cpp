#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint32_t lut[256];
    for (int i = 0; i < 256; ++i) {
        uint8_t low = i & 0x0F;
        uint8_t high = i >> 4;
        uint32_t sum = 0;
        if (low > threshold) sum += low;
        if (high > threshold) sum += high;
        lut[i] = sum;
    }
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint64_t result = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < n; ++i) {
            sum += lut[data[i]];
        }
        result = sum;
    }
    return result;
}
