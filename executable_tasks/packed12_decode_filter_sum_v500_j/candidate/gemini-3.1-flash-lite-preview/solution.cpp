#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    // Process in chunks of 3 bytes (2 integers)
    // We can process the entire buffer in one pass per iteration
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        const uint8_t* p = packed.data();
        const uint8_t* end = p + (n - (n % 3));

        for (; p < end; p += 3) {
            // Decode two 12-bit integers from 3 bytes
            // Byte 0: [0-7]
            // Byte 1: [8-11] (low) | [0-3] (high)
            // Byte 2: [4-11] (high)
            uint32_t a = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0F) << 8);
            uint32_t b = (static_cast<uint32_t>(p[1] >> 4)) | (static_cast<uint32_t>(p[2]) << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }

    return total_sum;
}