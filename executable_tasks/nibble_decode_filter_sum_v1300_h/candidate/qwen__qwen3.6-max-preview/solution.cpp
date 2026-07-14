#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t b = data[i];
            uint32_t low = b & 0x0Fu;
            uint32_t high = b >> 4u;
            current_sum += (low > threshold) ? low : 0;
            current_sum += (high > threshold) ? high : 0;
        }
        sum = current_sum;
    }
    return sum;
}