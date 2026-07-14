#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* __restrict data = packed.data();
    const std::size_t size = packed.size();
    const std::size_t num_pairs = size / 3u;
    const std::size_t main_pairs = (num_pairs / 8u) * 8u;
    const uint32_t thr = static_cast<uint32_t>(threshold);
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;
        
        // Process 8 pairs (16 values) = 24 bytes at a time
        for (std::size_t p = 0; p < main_pairs; p += 8) {
            // Pair 0
            uint32_t b0 = data[i];
            uint32_t b1 = data[i + 1];
            uint32_t b2 = data[i + 2];
            uint32_t a0 = b0 | ((b1 & 0x0fu) << 8u);
            uint32_t v0 = (b1 >> 4u) | (b2 << 4u);
            
            // Pair 1
            uint32_t b3 = data[i + 3];
            uint32_t b4 = data[i + 4];
            uint32_t b5 = data[i + 5];
            uint32_t a1 = b3 | ((b4 & 0x0fu) << 8u);
            uint32_t v1 = (b4 >> 4u) | (b5 << 4u);
            
            // Pair 2
            uint32_t b6 = data[i + 6];
            uint32_t b7 = data[i + 7];
            uint32_t b8 = data[i + 8];
            uint32_t a2 = b6 | ((b7 & 0x0fu) << 8u);
            uint32_t v2 = (b7 >> 4u) | (b8 << 4u);
            
            // Pair 3
            uint32_t b9 = data[i + 9];
            uint32_t b10 = data[i + 10];
            uint32_t b11 = data[i + 11];
            uint32_t a3 = b9 | ((b10 & 0x0fu) << 8u);
            uint32_t v3 = (b10 >> 4u) | (b11 << 4u);
            
            // Pair 4
            uint32_t b12 = data[i + 12];
            uint32_t b13 = data[i + 13];
            uint32_t b14 = data[i + 14];
            uint32_t a4 = b12 | ((b13 & 0x0fu) << 8u);
            uint32_t v4 = (b13 >> 4u) | (b14 << 4u);
            
            // Pair 5
            uint32_t b15 = data[i + 15];
            uint32_t b16 = data[i + 16];
            uint32_t b17 = data[i + 17];
            uint32_t a5 = b15 | ((b16 & 0x0fu) << 8u);
            uint32_t v5 = (b16 >> 4u) | (b17 << 4u);
            
            // Pair 6
            uint32_t b18 = data[i + 18];
            uint32_t b19 = data[i + 19];
            uint32_t b20 = data[i + 20];
            uint32_t a6 = b18 | ((b19 & 0x0fu) << 8u);
            uint32_t v6 = (b19 >> 4u) | (b20 << 4u);
            
            // Pair 7
            uint32_t b21 = data[i + 21];
            uint32_t b22 = data[i + 22];
            uint32_t b23 = data[i + 23];
            uint32_t a7 = b21 | ((b22 & 0x0fu) << 8u);
            uint32_t v7 = (b22 >> 4u) | (b23 << 4u);
            
            i += 24;
            
            // Branchless conditional add using arithmetic
            local_sum += (a0 > thr) ? a0 : 0u;
            local_sum += (v0 > thr) ? v0 : 0u;
            local_sum += (a1 > thr) ? a1 : 0u;
            local_sum += (v1 > thr) ? v1 : 0u;
            local_sum += (a2 > thr) ? a2 : 0u;
            local_sum += (v2 > thr) ? v2 : 0u;
            local_sum += (a3 > thr) ? a3 : 0u;
            local_sum += (v3 > thr) ? v3 : 0u;
            local_sum += (a4 > thr) ? a4 : 0u;
            local_sum += (v4 > thr) ? v4 : 0u;
            local_sum += (a5 > thr) ? a5 : 0u;
            local_sum += (v5 > thr) ? v5 : 0u;
            local_sum += (a6 > thr) ? a6 : 0u;
            local_sum += (v6 > thr) ? v6 : 0u;
            local_sum += (a7 > thr) ? a7 : 0u;
            local_sum += (v7 > thr) ? v7 : 0u;
        }
        
        // Handle remaining pairs
        for (std::size_t p = main_pairs; p < num_pairs; ++p) {
            uint32_t b0 = data[i];
            uint32_t b1 = data[i + 1];
            uint32_t b2 = data[i + 2];
            uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
            uint32_t b = (b1 >> 4u) | (b2 << 4u);
            i += 3;
            
            local_sum += (a > thr) ? a : 0u;
            local_sum += (b > thr) ? b : 0u;
        }
        
        sum = local_sum;
    }
    
    return sum;
}