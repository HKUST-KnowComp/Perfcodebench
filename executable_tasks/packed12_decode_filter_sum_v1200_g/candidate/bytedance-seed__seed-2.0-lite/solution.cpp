#include "interface.h"
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* bytes = packed.data();
    const std::size_t total_bytes = packed.size();
    const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;
        // Process full 3-byte triplets only, matching original bounds safety
        for (; i + 2 < total_bytes; i += 3) {
            // Exact same 12-bit decoding logic as baseline
            const uint32_t a = static_cast<uint32_t>(bytes[i]) |
                               (static_cast<uint32_t>(bytes[i+1] & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(bytes[i+1] >> 4)) |
                               (static_cast<uint32_t>(bytes[i+2]) << 4);
            // Fused threshold check and accumulation, no intermediate storage
            if (a > threshold_u32) sum += a;
            if (b > threshold_u32) sum += b;
        }
    }
    return sum;
}