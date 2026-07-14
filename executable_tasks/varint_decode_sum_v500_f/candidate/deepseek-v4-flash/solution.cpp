#include "interface.h"
#include <cstddef>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const uint8_t* ptr = encoded.data();
    const uint8_t* end = ptr + encoded.size();
    while (ptr < end) {
        uint64_t value = 0;
        uint64_t shift = 0;
        while (true) {
            uint8_t byte = *ptr++;
            value |= static_cast<uint64_t>(byte & 0x7F) << shift;
            if ((byte & 0x80) == 0) break;
            shift += 7;
        }
        sum += value;
    }
    return sum;
}