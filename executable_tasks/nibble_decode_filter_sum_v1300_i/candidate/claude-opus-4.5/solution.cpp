#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    const uint32_t thr = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;

        // Process 8 bytes at a time (16 nibbles)
        const std::size_t n8 = n & ~static_cast<std::size_t>(7);
        for (; i < n8; i += 8) {
            uint64_t chunk;
            // Load 8 bytes
            chunk = static_cast<uint64_t>(data[i])
                  | (static_cast<uint64_t>(data[i + 1]) << 8)
                  | (static_cast<uint64_t>(data[i + 2]) << 16)
                  | (static_cast<uint64_t>(data[i + 3]) << 24)
                  | (static_cast<uint64_t>(data[i + 4]) << 32)
                  | (static_cast<uint64_t>(data[i + 5]) << 40)
                  | (static_cast<uint64_t>(data[i + 6]) << 48)
                  | (static_cast<uint64_t>(data[i + 7]) << 56);

            // Extract and process all 16 nibbles
            uint32_t n0  = (chunk >>  0) & 0xF;
            uint32_t n1  = (chunk >>  4) & 0xF;
            uint32_t n2  = (chunk >>  8) & 0xF;
            uint32_t n3  = (chunk >> 12) & 0xF;
            uint32_t n4  = (chunk >> 16) & 0xF;
            uint32_t n5  = (chunk >> 20) & 0xF;
            uint32_t n6  = (chunk >> 24) & 0xF;
            uint32_t n7  = (chunk >> 28) & 0xF;
            uint32_t n8v = (chunk >> 32) & 0xF;
            uint32_t n9  = (chunk >> 36) & 0xF;
            uint32_t n10 = (chunk >> 40) & 0xF;
            uint32_t n11 = (chunk >> 44) & 0xF;
            uint32_t n12 = (chunk >> 48) & 0xF;
            uint32_t n13 = (chunk >> 52) & 0xF;
            uint32_t n14 = (chunk >> 56) & 0xF;
            uint32_t n15 = (chunk >> 60) & 0xF;

            // Conditional adds (compiler will optimize to branchless)
            if (n0  > thr) local_sum += n0;
            if (n1  > thr) local_sum += n1;
            if (n2  > thr) local_sum += n2;
            if (n3  > thr) local_sum += n3;
            if (n4  > thr) local_sum += n4;
            if (n5  > thr) local_sum += n5;
            if (n6  > thr) local_sum += n6;
            if (n7  > thr) local_sum += n7;
            if (n8v > thr) local_sum += n8v;
            if (n9  > thr) local_sum += n9;
            if (n10 > thr) local_sum += n10;
            if (n11 > thr) local_sum += n11;
            if (n12 > thr) local_sum += n12;
            if (n13 > thr) local_sum += n13;
            if (n14 > thr) local_sum += n14;
            if (n15 > thr) local_sum += n15;
        }

        // Handle remaining bytes
        for (; i < n; ++i) {
            uint32_t lo = data[i] & 0xF;
            uint32_t hi = data[i] >> 4;
            if (lo > thr) local_sum += lo;
            if (hi > thr) local_sum += hi;
        }

        sum = local_sum;
    }

    return sum;
}
