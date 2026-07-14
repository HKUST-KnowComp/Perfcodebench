#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* ptr = data;
        const uint8_t* const end = data + size;
        uint64_t iter_sum = 0;
        while (ptr < end) {
            uint64_t value = 0;
            unsigned shift = 0;
            uint8_t byte;
            do {
                byte = *ptr++;
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                shift += 7;
            } while (byte & 0x80);
            iter_sum += value;
        }
        sum = iter_sum;
    }
    return sum;
}
