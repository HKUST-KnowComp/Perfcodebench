#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t final_sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t i = 0;
        while (i < len) {
            const uint8_t b0 = data[i];
            if (!(b0 & 0x80u)) {
                // Fast path: 1-byte varint
                sum += b0;
                i += 1;
                continue;
            }
            const uint8_t b1 = data[i+1];
            if (!(b1 & 0x80u)) {
                // Very common 2-byte varint fast path
                const uint32_t val = (static_cast<uint32_t>(b0 & 0x7Fu)) | (static_cast<uint32_t>(b1) << 7);
                sum += val;
                i += 2;
                continue;
            }
            // Rare fallback: general decoding for varints longer than 2 bytes
            uint32_t value = 0;
            uint32_t shift = 0;
            for (;;) {
                const uint8_t byte = data[i++];
                value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
                if (!(byte & 0x80u)) break;
                shift += 7u;
            }
            sum += value;
        }
        final_sum = sum;
    }
    return final_sum;
}