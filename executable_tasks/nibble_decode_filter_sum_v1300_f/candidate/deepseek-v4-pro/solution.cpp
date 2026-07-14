#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    // Precompute a lookup table for all possible bytes given the threshold.
    // table[b] = sum of nibbles of b that are > threshold.
    uint32_t table[256];
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    for (int b = 0; b < 256; ++b) {
        uint32_t low  = b & 0xF;
        uint32_t high = b >> 4;
        uint32_t s = 0;
        if (low > thresh)  s += low;
        if (high > thresh) s += high;
        table[b] = s;
    }

    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            iter_sum += table[data[i]];
        }
        sum = iter_sum;
    }
    return sum;
}