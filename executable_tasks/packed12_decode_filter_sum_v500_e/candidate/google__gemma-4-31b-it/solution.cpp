#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    uint64_t total_sum = 0;
    const uint32_t u_threshold = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Process in chunks to help the compiler vectorize or unroll
        for (; i + 2 < size; i += 3) {
            const uint8_t p0 = data[i];
            const uint8_t p1 = data[i + 1];
            const uint8_t p2 = data[i + 2];

            // Decode first 12-bit value: p0 (8 bits) | (p1 & 0x0F) (4 bits)
            const uint32_t a = static_cast<uint32_t>(p0) | (static_cast<uint32_t>(p1 & 0x0Fu) << 8u);
            // Decode second 12-bit value: (p1 >> 4) (4 bits) | (p2 << 4) (8 bits)
            const uint32_t b = static_cast<uint32_t>(p1 >> 4u) | (static_cast<uint32_t>(p2) << 4u);

            if (a > u_threshold) current_sum += a;
            if (b > u_threshold) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}