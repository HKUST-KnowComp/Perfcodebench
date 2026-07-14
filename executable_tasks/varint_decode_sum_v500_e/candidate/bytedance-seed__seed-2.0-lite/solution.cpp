#include "interface.h"
#include <cstddef>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* buf = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;
        while (i < len) {
            uint32_t value = 0;
            uint32_t shift = 0;
            for (;;) {
                const uint8_t byte = buf[i++];
                value |= static_cast<uint32_t>(byte & 127u) << shift;
                if ((byte & 128u) == 0u) {
                    break;
                }
                shift += 7u;
            }
            sum += static_cast<uint64_t>(value);
        }
    }
    return sum;
}