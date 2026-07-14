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
        uint64_t value;
        uint8_t byte = *ptr++;
        
        if (__builtin_expect((byte & 0x80) == 0, 1)) {
            // 1-byte varint (most common case)
            value = byte;
        } else {
            value = byte & 0x7F;
            byte = *ptr++;
            if (__builtin_expect((byte & 0x80) == 0, 1)) {
                // 2-byte varint
                value |= static_cast<uint64_t>(byte) << 7;
            } else {
                value |= static_cast<uint64_t>(byte & 0x7F) << 7;
                byte = *ptr++;
                if (__builtin_expect((byte & 0x80) == 0, 1)) {
                    // 3-byte varint
                    value |= static_cast<uint64_t>(byte) << 14;
                } else {
                    value |= static_cast<uint64_t>(byte & 0x7F) << 14;
                    byte = *ptr++;
                    if (__builtin_expect((byte & 0x80) == 0, 1)) {
                        // 4-byte varint
                        value |= static_cast<uint64_t>(byte) << 21;
                    } else {
                        value |= static_cast<uint64_t>(byte & 0x7F) << 21;
                        byte = *ptr++;
                        if (__builtin_expect((byte & 0x80) == 0, 1)) {
                            // 5-byte varint
                            value |= static_cast<uint64_t>(byte) << 28;
                        } else {
                            // 6+ byte varint (rare for uint32, but handle for correctness)
                            value |= static_cast<uint64_t>(byte & 0x7F) << 28;
                            uint32_t shift = 35;
                            do {
                                byte = *ptr++;
                                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                                shift += 7;
                            } while (byte & 0x80);
                        }
                    }
                }
            }
        }
        sum += value;
    }
    
    return sum * static_cast<uint64_t>(iters);
}