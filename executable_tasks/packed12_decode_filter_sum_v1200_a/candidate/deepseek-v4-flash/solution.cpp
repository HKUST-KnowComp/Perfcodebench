#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    // No intermediate vector needed; decode and filter on the fly.
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t n = packed.size();
    uint64_t result_sum = 0;  // will hold the last iteration's sum

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        // Process three bytes at a time -> two 12-bit values
        for (std::size_t i = 0; i < n; i += 3) {
            uint8_t lo = packed[i];
            uint8_t mid = packed[i + 1];
            uint8_t hi = packed[i + 2];

            // Value 1: low 8 bits from lo, high 4 bits from low nibble of mid
            uint32_t v1 = static_cast<uint32_t>(lo) | (static_cast<uint32_t>(mid & 0x0F) << 8);
            // Value 2: low 4 bits from high nibble of mid, high 8 bits from hi
            uint32_t v2 = (static_cast<uint32_t>(mid >> 4)) | (static_cast<uint32_t>(hi) << 4);

            if (v1 > thresh) sum += v1;
            if (v2 > thresh) sum += v2;
        }
        result_sum = sum;
    }
    return result_sum;
}
