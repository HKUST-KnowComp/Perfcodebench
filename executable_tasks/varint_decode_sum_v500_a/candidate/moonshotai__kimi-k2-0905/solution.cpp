#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* __restrict data = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* ptr = data;
        const uint8_t* end = data + len;
        uint64_t sum = 0;

        while (ptr < end) {
            uint64_t value = 0;
            unsigned shift = 0;
            uint8_t byte;
            do {
                byte = *ptr++;
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                shift += 7;
            } while (byte & 0x80);
            sum += value;
        }
        total = sum;   // last iteration’s sum is the required result
    }
    return total;
}