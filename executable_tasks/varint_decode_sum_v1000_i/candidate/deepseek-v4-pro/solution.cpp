#include "interface.h"
#include <cstddef>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const std::size_t size = encoded.size();
    if (size == 0 || iters <= 0) return 0;

    const uint8_t* const data = encoded.data();
    const uint8_t* const end = data + size;
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        uint64_t sum = 0;
        while (p < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            uint8_t byte;
            do {
                byte = *p++;
                value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
                shift += 7u;
            } while (byte & 0x80u);
            sum += value;
        }
        total_sum = sum;
    }
    return total_sum;
}
