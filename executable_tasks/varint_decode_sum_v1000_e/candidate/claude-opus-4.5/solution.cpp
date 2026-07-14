#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (encoded.empty()) return 0;
    
    const uint8_t* __restrict__ data = encoded.data();
    const uint8_t* const end = data + encoded.size();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* __restrict__ ptr = data;
        uint_fast64_t local_sum = 0;
        
        while (ptr < end) {
            // Fast path: single byte varint (value < 128)
            uint8_t byte = *ptr++;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                local_sum += byte;
                continue;
            }
            
            // Multi-byte varint
            uint64_t value = byte & 0x7F;
            uint32_t shift = 7;
            
            // Unrolled loop for remaining bytes
            byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                local_sum += value;
                continue;
            }
            shift += 7;
            
            byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                local_sum += value;
                continue;
            }
            shift += 7;
            
            byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                local_sum += value;
                continue;
            }
            shift += 7;
            
            byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                local_sum += value;
                continue;
            }
            shift += 7;
            
            // Handle remaining bytes (up to 10 bytes for uint64)
            while (ptr < end) {
                byte = *ptr++;
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                if ((byte & 0x80) == 0) {
                    break;
                }
                shift += 7;
            }
            local_sum += value;
        }
        
        sum = local_sum;
    }
    
    return sum;
}