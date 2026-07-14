#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict__ data = packed.data();
    const std::size_t size = packed.size();
    const std::size_t num_pairs = size / 3u;
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;
        
        // Process 4 triplets (12 bytes) at a time = 8 values
        const std::size_t fast_end = (num_pairs / 4u) * 4u;
        
        for (; i < fast_end; i += 4) {
            const std::size_t base = i * 3u;
            
            // Load bytes for 4 triplets (8 12-bit values)
            const uint32_t b0 = data[base];
            const uint32_t b1 = data[base + 1];
            const uint32_t b2 = data[base + 2];
            const uint32_t b3 = data[base + 3];
            const uint32_t b4 = data[base + 4];
            const uint32_t b5 = data[base + 5];
            const uint32_t b6 = data[base + 6];
            const uint32_t b7 = data[base + 7];
            const uint32_t b8 = data[base + 8];
            const uint32_t b9 = data[base + 9];
            const uint32_t b10 = data[base + 10];
            const uint32_t b11 = data[base + 11];
            
            // Decode 8 values
            const uint32_t v0 = b0 | ((b1 & 0x0fu) << 8u);
            const uint32_t v1 = (b1 >> 4u) | (b2 << 4u);
            const uint32_t v2 = b3 | ((b4 & 0x0fu) << 8u);
            const uint32_t v3 = (b4 >> 4u) | (b5 << 4u);
            const uint32_t v4 = b6 | ((b7 & 0x0fu) << 8u);
            const uint32_t v5 = (b7 >> 4u) | (b8 << 4u);
            const uint32_t v6 = b9 | ((b10 & 0x0fu) << 8u);
            const uint32_t v7 = (b10 >> 4u) | (b11 << 4u);
            
            // Branchless accumulation using arithmetic
            local_sum += (v0 > thresh) ? v0 : 0u;
            local_sum += (v1 > thresh) ? v1 : 0u;
            local_sum += (v2 > thresh) ? v2 : 0u;
            local_sum += (v3 > thresh) ? v3 : 0u;
            local_sum += (v4 > thresh) ? v4 : 0u;
            local_sum += (v5 > thresh) ? v5 : 0u;
            local_sum += (v6 > thresh) ? v6 : 0u;
            local_sum += (v7 > thresh) ? v7 : 0u;
        }
        
        // Handle remaining pairs
        for (; i < num_pairs; ++i) {
            const std::size_t base = i * 3u;
            const uint32_t b0 = data[base];
            const uint32_t b1 = data[base + 1];
            const uint32_t b2 = data[base + 2];
            
            const uint32_t v0 = b0 | ((b1 & 0x0fu) << 8u);
            const uint32_t v1 = (b1 >> 4u) | (b2 << 4u);
            
            local_sum += (v0 > thresh) ? v0 : 0u;
            local_sum += (v1 > thresh) ? v1 : 0u;
        }
        
        sum = local_sum;
    }
    
    return sum;
}