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
        
        // Process 4 bytes at a time for better instruction-level parallelism
        const std::size_t n4 = n & ~std::size_t(3);
        for (; i < n4; i += 4) {
            uint32_t lo0 = data[i] & 15u;
            uint32_t hi0 = data[i] >> 4u;
            uint32_t lo1 = data[i + 1] & 15u;
            uint32_t hi1 = data[i + 1] >> 4u;
            uint32_t lo2 = data[i + 2] & 15u;
            uint32_t hi2 = data[i + 2] >> 4u;
            uint32_t lo3 = data[i + 3] & 15u;
            uint32_t hi3 = data[i + 3] >> 4u;
            
            if (lo0 > thresh) local_sum += lo0;
            if (hi0 > thresh) local_sum += hi0;
            if (lo1 > thresh) local_sum += lo1;
            if (hi1 > thresh) local_sum += hi1;
            if (lo2 > thresh) local_sum += lo2;
            if (hi2 > thresh) local_sum += hi2;
            if (lo3 > thresh) local_sum += lo3;
            if (hi3 > thresh) local_sum += hi3;
        }
        
        // Handle remaining bytes
        for (; i < n; ++i) {
            uint32_t lo = data[i] & 15u;
            uint32_t hi = data[i] >> 4u;
            if (lo > thresh) local_sum += lo;
            if (hi > thresh) local_sum += hi;
        }
        
        sum = local_sum;
    }
    
    return sum;
}