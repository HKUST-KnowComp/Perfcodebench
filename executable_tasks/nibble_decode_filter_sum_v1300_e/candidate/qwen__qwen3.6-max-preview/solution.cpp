#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    // Precompute a lookup table: for each possible byte value, store the sum of
    // its two nibbles that exceed the threshold. Max possible sum per byte is 30.
    uint32_t lut[256];
    for (int i = 0; i < 256; ++i) {
        uint32_t lo = i & 0x0F;
        uint32_t hi = i >> 4;
        lut[i] = (lo > threshold ? lo : 0) + (hi > threshold ? hi : 0);
    }

    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    std::size_t n = packed.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            current_sum += lut[data[i]];
        }
        sum = current_sum;
    }
    return sum;
}
