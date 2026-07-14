#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    
    // Precompute a lookup table: maps each possible byte to the sum of its 
    // two nibbles that exceed the threshold. Fits entirely in L1 cache.
    uint8_t lut[256];
    for (int i = 0; i < 256; ++i) {
        uint32_t lo = i & 15u;
        uint32_t hi = i >> 4u;
        lut[i] = static_cast<uint8_t>((lo > threshold ? lo : 0) + (hi > threshold ? hi : 0));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            current_sum += lut[data[i]];
        }
        sum = current_sum;
    }
    return sum;
}