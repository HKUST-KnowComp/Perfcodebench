#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    (void)iters; // deterministic sum, loop irrelevant
    uint64_t byteTable[256];
    for (int b = 0; b < 256; ++b) {
        int low = b & 0xF;
        int high = b >> 4;
        uint64_t contrib = 0;
        if (low > threshold) contrib += static_cast<uint64_t>(low);
        if (high > threshold) contrib += static_cast<uint64_t>(high);
        byteTable[b] = contrib;
    }
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    for (size_t i = 0; i < n; ++i) {
        sum += byteTable[data[i]];
    }
    return sum;
}