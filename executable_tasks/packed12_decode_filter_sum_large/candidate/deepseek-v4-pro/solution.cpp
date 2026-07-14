#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    const std::size_t numTriplets = n / 3; // only full triplets

    const uint32_t thr = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (std::size_t i = 0; i < numTriplets; ++i) {
        const std::size_t idx = i * 3;
        const uint32_t byte0 = data[idx];
        const uint32_t byte1 = data[idx + 1];
        const uint32_t byte2 = data[idx + 2];

        const uint32_t a = byte0 | ((byte1 & 0x0Fu) << 8);
        const uint32_t b = (byte1 >> 4) | (byte2 << 4);

        // Branchless conditional add (compiler will use cmov)
        sum += (a > thr) ? a : 0;
        sum += (b > thr) ? b : 0;
    }

    return sum;
}