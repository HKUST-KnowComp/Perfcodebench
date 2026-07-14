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
            
            // Triplet 0
            uint32_t b0 = data[base];
            uint32_t b1 = data[base + 1];
            uint32_t b2 = data[base + 2];
            uint32_t a0 = b0 | ((b1 & 0x0fu) << 8u);
            uint32_t a1 = (b1 >> 4u) | (b2 << 4u);
            
            // Triplet 1
            uint32_t b3 = data[base + 3];
            uint32_t b4 = data[base + 4];
            uint32_t b5 = data[base + 5];
            uint32_t a2 = b3 | ((b4 & 0x0fu) << 8u);
            uint32_t a3 = (b4 >> 4u) | (b5 << 4u);
            
            // Triplet 2
            uint32_t b6 = data[base + 6];
            uint32_t b7 = data[base + 7];
            uint32_t b8 = data[base + 8];
            uint32_t a4 = b6 | ((b7 & 0x0fu) << 8u);
            uint32_t a5 = (b7 >> 4u) | (b8 << 4u);
            
            // Triplet 3
            uint32_t b9 = data[base + 9];
            uint32_t b10 = data[base + 10];
            uint32_t b11 = data[base + 11];
            uint32_t a6 = b9 | ((b10 & 0x0fu) << 8u);
            uint32_t a7 = (b10 >> 4u) | (b11 << 4u);
            
            // Accumulate with threshold check
            if (a0 > thresh) local_sum += a0;
            if (a1 > thresh) local_sum += a1;
            if (a2 > thresh) local_sum += a2;
            if (a3 > thresh) local_sum += a3;
            if (a4 > thresh) local_sum += a4;
            if (a5 > thresh) local_sum += a5;
            if (a6 > thresh) local_sum += a6;
            if (a7 > thresh) local_sum += a7;
        }
        
        // Handle remaining pairs
        for (; i < num_pairs; ++i) {
            const std::size_t base = i * 3u;
            uint32_t b0 = data[base];
            uint32_t b1 = data[base + 1];
            uint32_t b2 = data[base + 2];
            uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
            uint32_t b = (b1 >> 4u) | (b2 << 4u);
            if (a > thresh) local_sum += a;
            if (b > thresh) local_sum += b;
        }
        
        sum = local_sum;
    }
    
    return sum;
}