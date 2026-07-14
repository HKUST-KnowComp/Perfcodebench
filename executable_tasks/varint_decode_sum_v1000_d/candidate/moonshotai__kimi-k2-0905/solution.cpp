#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;
        while (i < len) {
            uint8_t byte = data[i++];
            if (byte < 0x80) {
                sum += byte;
                continue;
            }
            uint64_t value = byte & 0x7F;
            byte = data[i++];
            if (byte < 0x80) {
                sum += value | (static_cast<uint64_t>(byte) << 7);
                continue;
            }
            value |= static_cast<uint64_t>(byte & 0x7F) << 7;
            uint32_t shift = 14;
            for (;;) {
                byte = data[i++];
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                if (byte < 0x80) break;
                shift += 7;
            }
            sum += value;
        }
        total = sum;
    }
    return total;
}