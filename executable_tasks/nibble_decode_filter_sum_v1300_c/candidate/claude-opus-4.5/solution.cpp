#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;

        // Process 8 bytes at a time (16 nibbles)
        const std::size_t n8 = n & ~std::size_t(7);
        while (i < n8) {
            uint64_t chunk;
            // Load 8 bytes
            chunk = static_cast<uint64_t>(data[i])
                  | (static_cast<uint64_t>(data[i+1]) << 8)
                  | (static_cast<uint64_t>(data[i+2]) << 16)
                  | (static_cast<uint64_t>(data[i+3]) << 24)
                  | (static_cast<uint64_t>(data[i+4]) << 32)
                  | (static_cast<uint64_t>(data[i+5]) << 40)
                  | (static_cast<uint64_t>(data[i+6]) << 48)
                  | (static_cast<uint64_t>(data[i+7]) << 56);

            // Extract low nibbles and high nibbles
            uint64_t lo = chunk & 0x0F0F0F0F0F0F0F0FULL;
            uint64_t hi = (chunk >> 4) & 0x0F0F0F0F0F0F0F0FULL;

            // Process each nibble
            for (int j = 0; j < 8; ++j) {
                uint8_t vlo = (lo >> (j * 8)) & 0xF;
                uint8_t vhi = (hi >> (j * 8)) & 0xF;
                if (vlo > threshold) local_sum += vlo;
                if (vhi > threshold) local_sum += vhi;
            }

            i += 8;
        }

        // Handle remaining bytes
        while (i < n) {
            uint8_t byte = data[i];
            uint8_t lo = byte & 0xF;
            uint8_t hi = byte >> 4;
            if (lo > threshold) local_sum += lo;
            if (hi > threshold) local_sum += hi;
            ++i;
        }

        sum = local_sum;
    }

    return sum;
}
