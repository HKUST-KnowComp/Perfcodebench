#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        // Process 3 bytes at a time to extract two 12-bit values
        for (size_t i = 0; i < size; i += 3) {
            uint8_t p0 = data[i];
            uint8_t p1 = data[i + 1];
            uint8_t p2 = data[i + 2];

            // Value A: p0 (8 bits) | (p1 & 0x0F) (4 bits)
            uint32_t a = static_cast<uint32_t>(p0) | (static_cast<uint32_t>(p1 & 0x0Fu) << 8);
            // Value B: (p1 >> 4) (4 bits) | p2 (8 bits)
            uint32_t b = static_cast<uint32_t>(p1 >> 4) | (static_cast<uint32_t>(p2) << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}