#include "interface.h"
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    uint64_t final_sum = 0;
    const std::size_t len = encoded.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        const uint8_t* ptr = encoded.data();
        const uint8_t* end = ptr + len;
        while (ptr < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            uint8_t byte;
            do {
                byte = *ptr++;
                value |= static_cast<uint32_t>(byte & 127u) << shift;
                shift += 7u;
            } while (byte & 128u);
            current_sum += value;
        }
        final_sum = current_sum;
    }
    return final_sum;
}