#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0) return 0;

    const uint8_t* data = encoded.data();
    std::size_t size = encoded.size();
    uint64_t sum = 0;
    std::size_t i = 0;

    while (i < size) {
        uint32_t value = 0;
        int shift = 0;
        while (true) {
            uint8_t byte = data[i++];
            value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
            if ((byte & 0x80u) == 0u) break;
            shift += 7;
        }
        sum += static_cast<uint64_t>(value);
    }
    return sum;
}