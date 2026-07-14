#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    (void)iters;
    if (packed.empty()) return 0;
    
    uint64_t sum = 0;
    const uint8_t* p = packed.data();
    size_t n = packed.size();
    size_t i = 0;
    
    // Unroll loop: process 4 chunks (12 bytes -> 8 values) per iteration
    for (; i + 11 < n; i += 12) {
        uint32_t a0 = p[i] | ((p[i+1] & 0x0F) << 8);
        uint32_t b0 = (p[i+1] >> 4) | (p[i+2] << 4);
        uint32_t a1 = p[i+3] | ((p[i+4] & 0x0F) << 8);
        uint32_t b1 = (p[i+4] >> 4) | (p[i+5] << 4);
        uint32_t a2 = p[i+6] | ((p[i+7] & 0x0F) << 8);
        uint32_t b2 = (p[i+7] >> 4) | (p[i+8] << 4);
        uint32_t a3 = p[i+9] | ((p[i+10] & 0x0F) << 8);
        uint32_t b3 = (p[i+10] >> 4) | (p[i+11] << 4);
        
        if (a0 > threshold) sum += a0;
        if (b0 > threshold) sum += b0;
        if (a1 > threshold) sum += a1;
        if (b1 > threshold) sum += b1;
        if (a2 > threshold) sum += a2;
        if (b2 > threshold) sum += b2;
        if (a3 > threshold) sum += a3;
        if (b3 > threshold) sum += b3;
    }
    
    // Handle remaining chunks
    for (; i + 2 < n; i += 3) {
        uint32_t a = p[i] | ((p[i+1] & 0x0F) << 8);
        uint32_t b = (p[i+1] >> 4) | (p[i+2] << 4);
        if (a > threshold) sum += a;
        if (b > threshold) sum += b;
    }
    
    return sum;
}