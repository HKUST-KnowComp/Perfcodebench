#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    if (iters <= 0 || packed.empty()) {
        return 0;
    }

    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();

    // Preserve the iters loop to match baseline workload expectations,
    // though the result is identical for every iteration.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;

        // Process 2 bytes (4 nibbles) per iteration to reduce loop overhead
        for (; i + 1 < n; i += 2) {
            uint8_t b0 = data[i];
            uint8_t b1 = data[i + 1];

            // Extract nibbles
            uint8_t n0 = b0 & 0xF;
            uint8_t n1 = b0 >> 4;
            uint8_t n2 = b1 & 0xF;
            uint8_t n3 = b1 >> 4;

            // Branchless accumulation: (condition) * value
            current_sum += (n0 > threshold) * n0;
            current_sum += (n1 > threshold) * n1;
            current_sum += (n2 > threshold) * n2;
            current_sum += (n3 > threshold) * n3;
        }

        // Handle remaining byte(s)
        for (; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t n0 = b & 0xF;
            uint8_t n1 = b >> 4;
            current_sum += (n0 > threshold) * n0;
            current_sum += (n1 > threshold) * n1;
        }

        // Match baseline behavior: sum is reset every iteration
        sum = current_sum;
    }

    return sum;
}