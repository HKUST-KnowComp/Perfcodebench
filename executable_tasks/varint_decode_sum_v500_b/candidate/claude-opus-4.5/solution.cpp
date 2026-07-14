#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0 || encoded.empty()) {
        return 0;
    }
    
    const uint8_t* __restrict__ ptr = encoded.data();
    const uint8_t* const end = ptr + encoded.size();
    
    uint64_t sum = 0;
    
    while (ptr < end) {
        uint64_t value = 0;
        uint32_t shift = 0;
        
        for (;;) {
            const uint8_t byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
            if (__builtin_expect((byte & 0x80u) == 0u, 1)) {
                break;
            }
            shift += 7u;
        }
        
        sum += value;
    }
    
    return sum * static_cast<uint64_t>(iters);
}