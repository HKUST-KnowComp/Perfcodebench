#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    const std::size_t num_pairs = size / 3u;
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
        
        std::size_t i = 0;
        const std::size_t unroll_limit = (num_pairs / 4u) * 4u;
        
        // Process 4 pairs (12 bytes) at a time with 4 accumulators
        for (; i < unroll_limit; i += 4u) {
            const uint8_t* p = data + i * 3u;
            
            // Pair 0
            uint32_t a0 = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0fu) << 8u);
            uint32_t b0 = (static_cast<uint32_t>(p[1]) >> 4u) | (static_cast<uint32_t>(p[2]) << 4u);
            
            // Pair 1
            uint32_t a1 = static_cast<uint32_t>(p[3]) | (static_cast<uint32_t>(p[4] & 0x0fu) << 8u);
            uint32_t b1 = (static_cast<uint32_t>(p[4]) >> 4u) | (static_cast<uint32_t>(p[5]) << 4u);
            
            // Pair 2
            uint32_t a2 = static_cast<uint32_t>(p[6]) | (static_cast<uint32_t>(p[7] & 0x0fu) << 8u);
            uint32_t b2 = (static_cast<uint32_t>(p[7]) >> 4u) | (static_cast<uint32_t>(p[8]) << 4u);
            
            // Pair 3
            uint32_t a3 = static_cast<uint32_t>(p[9]) | (static_cast<uint32_t>(p[10] & 0x0fu) << 8u);
            uint32_t b3 = (static_cast<uint32_t>(p[10]) >> 4u) | (static_cast<uint32_t>(p[11]) << 4u);
            
            // Branchless accumulation using arithmetic
            acc0 += (a0 > thresh) ? a0 : 0u;
            acc1 += (b0 > thresh) ? b0 : 0u;
            acc2 += (a1 > thresh) ? a1 : 0u;
            acc3 += (b1 > thresh) ? b1 : 0u;
            acc0 += (a2 > thresh) ? a2 : 0u;
            acc1 += (b2 > thresh) ? b2 : 0u;
            acc2 += (a3 > thresh) ? a3 : 0u;
            acc3 += (b3 > thresh) ? b3 : 0u;
        }
        
        // Handle remaining pairs
        for (; i < num_pairs; ++i) {
            const uint8_t* p = data + i * 3u;
            uint32_t a = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1] & 0x0fu) << 8u);
            uint32_t b = (static_cast<uint32_t>(p[1]) >> 4u) | (static_cast<uint32_t>(p[2]) << 4u);
            acc0 += (a > thresh) ? a : 0u;
            acc1 += (b > thresh) ? b : 0u;
        }
        
        sum = acc0 + acc1 + acc2 + acc3;
    }
    
    return sum;
}