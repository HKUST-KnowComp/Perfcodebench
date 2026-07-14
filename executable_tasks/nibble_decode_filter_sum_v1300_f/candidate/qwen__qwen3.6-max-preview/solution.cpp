#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    // Precompute a lookup table: maps each byte to the sum of its nibbles that exceed the threshold.
    // Table fits entirely in L1 cache (256 bytes), enabling extremely fast decode+filter fusion.
    uint8_t table[256];
    for (int i = 0; i < 256; ++i) {
        uint8_t lo = i & 0x0F;
        uint8_t hi = i >> 4;
        table[i] = (lo > threshold ? lo : 0) + (hi > threshold ? hi : 0);
    }

    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    std::size_t len = packed.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;
        // Unrolled main loop for better instruction-level parallelism
        for (; i + 3 < len; i += 4) {
            sum += table[data[i]];
            sum += table[data[i + 1]];
            sum += table[data[i + 2]];
            sum += table[data[i + 3]];
        }
        // Handle remaining bytes
        for (; i < len; ++i) {
            sum += table[data[i]];
        }
    }
    return sum;
}
