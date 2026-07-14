#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint8_t* data = packed.data();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        // Process 3 bytes to get 2 values of 12 bits each
        for (size_t i = 0; i < n; i += 3) {
            uint8_t p0 = data[i];
            uint8_t p1 = data[i + 1];
            uint8_t p2 = data[i + 2];

            // Value A: p0 (8 bits) | (p1 & 0x0F) << 8
            uint32_t a = p0 | ((p1 & 0x0Fu) << 8);
            // Value B: (p1 >> 4) (4 bits) | (p2 << 4) (8 bits)
            uint32_t b = (p1 >> 4) | (p2 << 4);

            if (a > threshold) {
                current_sum += a;
            }
            if (b > threshold) {
                current_sum += b;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}