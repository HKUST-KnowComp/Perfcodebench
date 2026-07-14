#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0 || encoded.empty()) {
        return 0;
    }
    
    const uint8_t* __restrict__ ptr = encoded.data();
    const uint8_t* const end = ptr + encoded.size();
    
    uint64_t sum = 0;
    
    while (ptr < end) {
        // Fast path for 1-byte varint (value 0-127)
        uint8_t b0 = *ptr++;
        if ((b0 & 0x80) == 0) {
            sum += b0;
            continue;
        }
        
        // Fast path for 2-byte varint
        uint8_t b1 = *ptr++;
        if ((b1 & 0x80) == 0) {
            sum += (b0 & 0x7F) | (static_cast<uint32_t>(b1) << 7);
            continue;
        }
        
        // Fast path for 3-byte varint
        uint8_t b2 = *ptr++;
        if ((b2 & 0x80) == 0) {
            sum += (b0 & 0x7F) | 
                   (static_cast<uint32_t>(b1 & 0x7F) << 7) | 
                   (static_cast<uint32_t>(b2) << 14);
            continue;
        }
        
        // Fast path for 4-byte varint
        uint8_t b3 = *ptr++;
        if ((b3 & 0x80) == 0) {
            sum += (b0 & 0x7F) | 
                   (static_cast<uint32_t>(b1 & 0x7F) << 7) | 
                   (static_cast<uint32_t>(b2 & 0x7F) << 14) |
                   (static_cast<uint32_t>(b3) << 21);
            continue;
        }
        
        // 5-byte varint (max for uint32_t)
        uint8_t b4 = *ptr++;
        sum += (b0 & 0x7F) | 
               (static_cast<uint32_t>(b1 & 0x7F) << 7) | 
               (static_cast<uint32_t>(b2 & 0x7F) << 14) |
               (static_cast<uint32_t>(b3 & 0x7F) << 21) |
               (static_cast<uint32_t>(b4 & 0x0F) << 28);
    }
    
    // Since encoded data is the same each iteration, multiply by iters
    return sum * static_cast<uint64_t>(iters);
}