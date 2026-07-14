#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        while (i < size) {
            uint8_t byte0 = data[i++];
            if (!(byte0 & 0x80)) {
                current_sum += byte0;
                continue;
            }
            
            // Most common case: 2 bytes
            uint8_t byte1 = data[i++];
            if (!(byte1 & 0x80)) {
                current_sum += (uint32_t)(byte0 & 0x7F) | ((uint32_t)(byte1 & 0x7F) << 7);
                continue;
            }

            // Fallback for 3+ bytes
            uint32_t value = (uint32_t)(byte0 & 0x7F) | ((uint32_t)(byte1 & 0x7F) << 7);
            uint32_t shift = 14;
            while (true) {
                uint8_t b = data[i++];
                value |= (uint32_t)(b & 0x7F) << shift;
                if (!(b & 0x80)) break;
                shift += 7;
            }
            current_sum += value;
        }
        total_sum = current_sum;
    }
    return total_sum;
}