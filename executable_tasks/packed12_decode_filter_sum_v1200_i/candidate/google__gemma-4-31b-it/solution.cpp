#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        const uint32_t thresh = static_cast<uint32_t>(threshold);
        
        // Process in chunks of 3 bytes to extract 2 values of 12 bits each
        for (size_t i = 0; i < size; i += 3) {
            const uint8_t p0 = data[i];
            const uint8_t p1 = data[i + 1];
            const uint8_t p2 = data[i + 2];

            // Value A: p0 (8 bits) + low 4 bits of p1
            const uint32_t a = static_cast<uint32_t>(p0) | (static_cast<uint32_t>(p1 & 0x0F) << 8);
            // Value B: high 4 bits of p1 + p2 (8 bits)
            const uint32_t b = static_cast<uint32_t>(p1 >> 4) | (static_cast<uint32_t>(p2) << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}