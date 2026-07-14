#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        const uint8_t* p = packed.data();
        size_t i = 0;
        
        // Process in chunks of 3 bytes (2 integers) to avoid allocations
        // Unrolling slightly for better instruction pipeline utilization
        for (; i + 2 < n; i += 3) {
            uint32_t b0 = p[i];
            uint32_t b1 = p[i + 1];
            uint32_t b2 = p[i + 2];

            uint32_t v1 = b0 | ((b1 & 0x0F) << 8);
            uint32_t v2 = (b1 >> 4) | (b2 << 4);

            if (v1 > thresh) current_iter_sum += v1;
            if (v2 > thresh) current_iter_sum += v2;
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}