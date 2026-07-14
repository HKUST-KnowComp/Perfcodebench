#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    std::size_t size = encoded.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        const uint8_t* end = data + size;
        uint64_t iter_sum = 0;
        while (p < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            uint8_t byte;
            do {
                byte = *p++;
                value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
                shift += 7u;
            } while (byte & 0x80u);
            iter_sum += value;
        }
        sum = iter_sum;
    }
    return sum;
}
