#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t lookup[256];
    for (int b = 0; b < 256; ++b) {
        uint8_t low = b & 0xF;
        uint8_t high = b >> 4;
        uint64_t contrib = 0;
        if (low > threshold) contrib += low;
        if (high > threshold) contrib += high;
        lookup[b] = contrib;
    }

    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (uint8_t byte : packed) {
            sum += lookup[byte];
        }
        final_sum = sum;
    }
    return final_sum;
}