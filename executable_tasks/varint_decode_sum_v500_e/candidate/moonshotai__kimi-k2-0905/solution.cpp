#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;
        while (i < len) {
            uint64_t val = 0;
            uint8_t byte;

            // fast path: 1-byte varint
            byte = data[i];
            if (byte < 0x80) {
                sum += byte;
                ++i;
                continue;
            }

            // unrolled decode: up to 4 bytes
            byte = data[i++];
            val = byte & 0x7F;
            byte = data[i++];
            val |= (byte & 0x7F) << 7;
            if (byte < 0x80) { sum += val; continue; }

            byte = data[i++];
            val |= (byte & 0x7F) << 14;
            if (byte < 0x80) { sum += val; continue; }

            byte = data[i++];
            val |= (byte & 0x7F) << 21;
            if (byte < 0x80) { sum += val; continue; }

            // remaining bytes (rare)
            uint32_t shift = 28;
            for (;;) {
                byte = data[i++];
                val |= static_cast<uint64_t>(byte & 0x7F) << shift;
                if (byte < 0x80) break;
                shift += 7;
            }
            sum += val;
        }
        total = sum;
    }
    return total;
}