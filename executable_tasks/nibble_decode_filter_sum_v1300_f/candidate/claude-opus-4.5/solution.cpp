#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

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
            uint32_t lo0 = (chunk) & 0xFu;
            uint32_t hi0 = (chunk >> 4) & 0xFu;
            uint32_t lo1 = (chunk >> 8) & 0xFu;
            uint32_t hi1 = (chunk >> 12) & 0xFu;
            uint32_t lo2 = (chunk >> 16) & 0xFu;
            uint32_t hi2 = (chunk >> 20) & 0xFu;
            uint32_t lo3 = (chunk >> 24) & 0xFu;
            uint32_t hi3 = (chunk >> 28) & 0xFu;
            uint32_t lo4 = (chunk >> 32) & 0xFu;
            uint32_t hi4 = (chunk >> 36) & 0xFu;
            uint32_t lo5 = (chunk >> 40) & 0xFu;
            uint32_t hi5 = (chunk >> 44) & 0xFu;
            uint32_t lo6 = (chunk >> 48) & 0xFu;
            uint32_t hi6 = (chunk >> 52) & 0xFu;
            uint32_t lo7 = (chunk >> 56) & 0xFu;
            uint32_t hi7 = (chunk >> 60) & 0xFu;

            // Conditional add using branchless arithmetic
            local_sum += (lo0 > thresh) ? lo0 : 0u;
            local_sum += (hi0 > thresh) ? hi0 : 0u;
            local_sum += (lo1 > thresh) ? lo1 : 0u;
            local_sum += (hi1 > thresh) ? hi1 : 0u;
            local_sum += (lo2 > thresh) ? lo2 : 0u;
            local_sum += (hi2 > thresh) ? hi2 : 0u;
            local_sum += (lo3 > thresh) ? lo3 : 0u;
            local_sum += (hi3 > thresh) ? hi3 : 0u;
            local_sum += (lo4 > thresh) ? lo4 : 0u;
            local_sum += (hi4 > thresh) ? hi4 : 0u;
            local_sum += (lo5 > thresh) ? lo5 : 0u;
            local_sum += (hi5 > thresh) ? hi5 : 0u;
            local_sum += (lo6 > thresh) ? lo6 : 0u;
            local_sum += (hi6 > thresh) ? hi6 : 0u;
            local_sum += (lo7 > thresh) ? lo7 : 0u;
            local_sum += (hi7 > thresh) ? hi7 : 0u;
        }

        // Handle remaining bytes
        for (; i < n; ++i) {
            uint32_t lo = data[i] & 0xFu;
            uint32_t hi = data[i] >> 4u;
            if (lo > thresh) local_sum += lo;
            if (hi > thresh) local_sum += hi;
        }

        sum = local_sum;
    }

    return sum;
}
