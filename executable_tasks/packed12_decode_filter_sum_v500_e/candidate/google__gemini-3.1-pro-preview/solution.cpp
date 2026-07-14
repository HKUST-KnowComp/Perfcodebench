#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (packed.empty()) return 0;

    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const size_t num_triplets = n / 3;
    const uint32_t thresh32 = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;

        // Unroll to process 2 triplets (4 values) at a time
        for (; i + 5 < n; i += 6) {
            // Triplet 1
            const uint32_t a1 = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i + 1] & 0x0Fu) << 8u);
            const uint32_t b1 = static_cast<uint32_t>(data[i + 1] >> 4u) | (static_cast<uint32_t>(data[i + 2]) << 4u);
            
            // Triplet 2
            const uint32_t a2 = static_cast<uint32_t>(data[i + 3]) | (static_cast<uint32_t>(data[i + 4] & 0x0Fu) << 8u);
            const uint32_t b2 = static_cast<uint32_t>(data[i + 4] >> 4u) | (static_cast<uint32_t>(data[i + 5]) << 4u);

            if (a1 > thresh32) current_sum += a1;
            if (b1 > thresh32) current_sum += b1;
            if (a2 > thresh32) current_sum += a2;
            if (b2 > thresh32) current_sum += b2;
        }

        // Handle remaining triplets
        for (; i + 2 < n; i += 3) {
            const uint32_t a = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i + 1] & 0x0Fu) << 8u);
            const uint32_t b = static_cast<uint32_t>(data[i + 1] >> 4u) | (static_cast<uint32_t>(data[i + 2]) << 4u);
            
            if (a > thresh32) current_sum += a;
            if (b > thresh32) current_sum += b;
        }
        
        total_sum = current_sum;
    }

    return total_sum;
}