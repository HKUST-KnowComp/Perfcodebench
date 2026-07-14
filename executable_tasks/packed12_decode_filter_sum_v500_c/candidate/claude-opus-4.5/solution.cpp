#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict__ data = packed.data();
    const std::size_t size = packed.size();
    const std::size_t num_pairs = size / 3u;
    const std::size_t aligned_pairs = (num_pairs / 4u) * 4u;
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;
        
        // Process 4 pairs (12 bytes) at a time for better throughput
        for (std::size_t p = 0; p < aligned_pairs; p += 4) {
            // Load 12 bytes and decode 8 values
            const uint8_t* ptr = data + i;
            
            // Pair 0: bytes 0,1,2
            uint32_t a0 = static_cast<uint32_t>(ptr[0]) | 
                         (static_cast<uint32_t>(ptr[1] & 0x0fu) << 8u);
            uint32_t b0 = (static_cast<uint32_t>(ptr[1]) >> 4u) | 
                         (static_cast<uint32_t>(ptr[2]) << 4u);
            
            // Pair 1: bytes 3,4,5
            uint32_t a1 = static_cast<uint32_t>(ptr[3]) | 
                         (static_cast<uint32_t>(ptr[4] & 0x0fu) << 8u);
            uint32_t b1 = (static_cast<uint32_t>(ptr[4]) >> 4u) | 
                         (static_cast<uint32_t>(ptr[5]) << 4u);
            
            // Pair 2: bytes 6,7,8
            uint32_t a2 = static_cast<uint32_t>(ptr[6]) | 
                         (static_cast<uint32_t>(ptr[7] & 0x0fu) << 8u);
            uint32_t b2 = (static_cast<uint32_t>(ptr[7]) >> 4u) | 
                         (static_cast<uint32_t>(ptr[8]) << 4u);
            
            // Pair 3: bytes 9,10,11
            uint32_t a3 = static_cast<uint32_t>(ptr[9]) | 
                         (static_cast<uint32_t>(ptr[10] & 0x0fu) << 8u);
            uint32_t b3 = (static_cast<uint32_t>(ptr[10]) >> 4u) | 
                         (static_cast<uint32_t>(ptr[11]) << 4u);
            
            // Branchless accumulation using arithmetic
            local_sum += (a0 > thresh) ? a0 : 0;
            local_sum += (b0 > thresh) ? b0 : 0;
            local_sum += (a1 > thresh) ? a1 : 0;
            local_sum += (b1 > thresh) ? b1 : 0;
            local_sum += (a2 > thresh) ? a2 : 0;
            local_sum += (b2 > thresh) ? b2 : 0;
            local_sum += (a3 > thresh) ? a3 : 0;
            local_sum += (b3 > thresh) ? b3 : 0;
            
            i += 12;
        }
        
        // Handle remaining pairs
        for (std::size_t p = aligned_pairs; p < num_pairs; ++p) {
            const uint8_t* ptr = data + i;
            uint32_t a = static_cast<uint32_t>(ptr[0]) | 
                        (static_cast<uint32_t>(ptr[1] & 0x0fu) << 8u);
            uint32_t b = (static_cast<uint32_t>(ptr[1]) >> 4u) | 
                        (static_cast<uint32_t>(ptr[2]) << 4u);
            
            local_sum += (a > thresh) ? a : 0;
            local_sum += (b > thresh) ? b : 0;
            
            i += 3;
        }
        
        sum = local_sum;
    }
    
    return sum;
}