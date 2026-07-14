#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (iters <= 0) return 0;
    const uint8_t* p = encoded.data();
    const uint8_t* end = p + encoded.size();
    uint64_t sum = 0;
    while (p < end) {
        uint32_t value = 0;
        unsigned int shift = 0;
        while (true) {
            uint8_t byte = *p++;
            value |= static_cast<uint32_t>(byte & 0x7F) << shift;
            if ((byte & 0x80) == 0) break;
            shift += 7;
        }
        sum += value;
    }
    return sum;
}