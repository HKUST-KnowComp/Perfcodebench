#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const std::size_t num_chunks = packed.size() / 3u;
    const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t chunk = 0; chunk < num_chunks; ++chunk) {
            const std::size_t i = chunk * 3u;
            const uint8_t b0 = packed[i];
            const uint8_t b1 = packed[i + 1u];
            const uint8_t b2 = packed[i + 2u];

            const uint32_t a = static_cast<uint32_t>(b0) | (static_cast<uint32_t>(b1 & 0x0fu) << 8u);
            const uint32_t b = (static_cast<uint32_t>(b1 >> 4u)) | (static_cast<uint32_t>(b2) << 4u);

            if (a > threshold_u32) sum += a;
            if (b > threshold_u32) sum += b;
        }
    }
    return sum;
}