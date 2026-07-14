#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    // Process in chunks of 3 bytes (2 integers)
    const size_t limit = (n / 3) * 3;
    const uint8_t* data = packed.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < limit; i += 3) {
            // Load 3 bytes as a 24-bit word
            const uint32_t b0 = data[i];
            const uint32_t b1 = data[i + 1];
            const uint32_t b2 = data[i + 2];

            // Decode two 12-bit integers
            const uint32_t v1 = b0 | ((b1 & 0x0F) << 8);
            const uint32_t v2 = (b1 >> 4) | (b2 << 4);

            // Filter and accumulate
            if (v1 > thresh) iter_sum += v1;
            if (v2 > thresh) iter_sum += v2;
        }
        total_sum = iter_sum;
    }

    return total_sum;
}