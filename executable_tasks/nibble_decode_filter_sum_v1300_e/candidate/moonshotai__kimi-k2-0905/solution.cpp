#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* __restrict p = packed.data();
    const std::size_t n         = packed.size();
    const uint32_t  thr32       = threshold;          // promote once

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t i = 0;

        // Process 4 bytes at a time
        for (; i + 3 < n; i += 4) {
            uint32_t b0 = p[i+0];
            uint32_t b1 = p[i+1];
            uint32_t b2 = p[i+2];
            uint32_t b3 = p[i+3];

            uint32_t lo0 = b0 & 15u;
            uint32_t hi0 = b0 >> 4u;
            uint32_t lo1 = b1 & 15u;
            uint32_t hi1 = b1 >> 4u;
            uint32_t lo2 = b2 & 15u;
            uint32_t hi2 = b2 >> 4u;
            uint32_t lo3 = b3 & 15u;
            uint32_t hi3 = b3 >> 4u;

            if (lo0 > thr32) sum += lo0;
            if (hi0 > thr32) sum += hi0;
            if (lo1 > thr32) sum += lo1;
            if (hi1 > thr32) sum += hi1;
            if (lo2 > thr32) sum += lo2;
            if (hi2 > thr32) sum += hi2;
            if (lo3 > thr32) sum += lo3;
            if (hi3 > thr32) sum += hi3;
        }

        // Handle remaining bytes
        for (; i < n; ++i) {
            uint32_t b = p[i];
            uint32_t lo = b & 15u;
            uint32_t hi = b >> 4u;
            if (lo > thr32) sum += lo;
            if (hi > thr32) sum += hi;
        }

        total = sum;   // last iteration wins
    }
    return total;
}