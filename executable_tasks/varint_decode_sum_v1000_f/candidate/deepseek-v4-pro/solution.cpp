#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0) return 0;

    const uint8_t* p = encoded.data();
    const uint8_t* end = p + encoded.size();
    uint64_t sum = 0;

    while (p < end) {
        uint8_t byte = *p++;
        // Fast path for single-byte varint (most common case)
        if ((byte & 0x80u) == 0u) {
            sum += static_cast<uint64_t>(byte);
            continue;
        }

        uint32_t value = byte & 0x7Fu;
        uint32_t shift = 7u;
        for (;;) {
            byte = *p++;
            value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
            if ((byte & 0x80u) == 0u) break;
            shift += 7u;
        }
        sum += static_cast<uint64_t>(value);
    }

    return sum;
}