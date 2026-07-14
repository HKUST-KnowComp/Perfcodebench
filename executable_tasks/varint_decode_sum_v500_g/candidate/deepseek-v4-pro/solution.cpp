#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t size = encoded.size();
    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t i = 0;
        while (i < size) {
            uint32_t value = 0;
            unsigned shift = 0;
            while (true) {
                const uint8_t byte = data[i++];
                value |= (byte & 0x7Fu) << shift;
                if (!(byte & 0x80u)) break;
                shift += 7;
            }
            sum += value;
        }
        final_sum = sum;
    }
    return final_sum;
}