#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    (void)iters;  // ignored; result is same for any iters
    const uint8_t* data = encoded.data();
    const uint8_t* end = data + encoded.size();
    uint64_t total = 0;
    const uint8_t* ptr = data;
    while (ptr != end) {
        uint32_t value = 0;
        uint32_t shift = 0;
        uint8_t byte;
        do {
            byte = *ptr++;
            value |= (uint32_t)(byte & 0x7F) << shift;
            shift += 7;
        } while (byte & 0x80);
        total += value;
    }
    return total;
}
