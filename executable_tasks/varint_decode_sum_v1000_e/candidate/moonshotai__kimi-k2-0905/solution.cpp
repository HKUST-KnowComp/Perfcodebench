#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t i = 0;
        while (i < len) {
            uint64_t value = 0;
            uint32_t shift = 0;
            uint8_t byte;
            do {
                byte = data[i++];
                value |= uint64_t(byte & 0x7F) << shift;
                shift += 7;
            } while (byte & 0x80);
            sum += value;
        }
        total = sum;
    }
    return total;
}