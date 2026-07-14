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
        
        // Process 4 triplets (8 values, 12 bytes) at a time
        const std::size_t fast_end = (num_pairs / 4u) * 4u;
        
        for (; i < fast_end; i += 4u) {
            const std::size_t base = i * 3u;
            
            // Load bytes for 4 triplets (12 bytes total)
            const uint8_t* p = data + base;
            
            // Triplet 0: bytes 0,1,2
            uint32_t a0 = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0fu) << 8u);
            uint32_t b0 = (static_cast<uint32_t>(p[1]) >> 4u) | (static_cast<uint32_t>(p[2]) << 4u);
            
            // Triplet 1: bytes 3,4,5
            uint32_t a1 = static_cast<uint32_t>(p[3]) | (static_cast<uint32_t>(p[4] & 0x0fu) << 8u);
            uint32_t b1 = (static_cast<uint32_t>(p[4]) >> 4u) | (static_cast<uint32_t>(p[5]) << 4u);
            
            // Triplet 2: bytes 6,7,8
            uint32_t a2 = static_cast<uint32_t>(p[6]) | (static_cast<uint32_t>(p[7] & 0x0fu) << 8u);
            uint32_t b2 = (static_cast<uint32_t>(p[7]) >> 4u) | (static_cast<uint32_t>(p[8]) << 4u);
            
            // Triplet 3: bytes 9,10,11
            uint32_t a3 = static_cast<uint32_t>(p[9]) | (static_cast<uint32_t>(p[10] & 0x0fu) << 8u);
            uint32_t b3 = (static_cast<uint32_t>(p[10]) >> 4u) | (static_cast<uint32_t>(p[11]) << 4u);
            
            // Branchless accumulation using arithmetic
            local_sum += (a0 > thresh) ? a0 : 0u;
            local_sum += (b0 > thresh) ? b0 : 0u;
            local_sum += (a1 > thresh) ? a1 : 0u;
            local_sum += (b1 > thresh) ? b1 : 0u;
            local_sum += (a2 > thresh) ? a2 : 0u;
            local_sum += (b2 > thresh) ? b2 : 0u;
            local_sum += (a3 > thresh) ? a3 : 0u;
            local_sum += (b3 > thresh) ? b3 : 0u;
        }
        
        // Handle remaining triplets
        for (; i < num_pairs; ++i) {
            const std::size_t base = i * 3u;
            const uint8_t* p = data + base;
            
            uint32_t a = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0fu) << 8u);
            uint32_t b = (static_cast<uint32_t>(p[1]) >> 4u) | (static_cast<uint32_t>(p[2]) << 4u);
            
            local_sum += (a > thresh) ? a : 0u;
            local_sum += (b > thresh) ? b : 0u;
        }
        
        sum = local_sum;
    }
    
    return sum;
}