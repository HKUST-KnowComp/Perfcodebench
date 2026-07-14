#include "interface.h"

#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters == 0) return 0;

    const uint8_t* data = packed.data();
    std::size_t size = packed.size();
    uint64_t sum = 0;

    // Process 3 bytes -> two 12-bit integers
    for (std::size_t i = 0; i < size; i += 3) {
        uint8_t b0 = data[i];
        uint8_t b1 = data[i + 1];
        uint8_t b2 = data[i + 2];

        uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
        uint32_t b = (b1 >> 4) | (b2 << 4);

        if (a > threshold) sum += a;
        if (b > threshold) sum += b;
    }

    return sum;
}
