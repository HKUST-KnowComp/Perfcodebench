#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t size = encoded.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        const uint8_t* end = data + size;
        uint64_t sum = 0;

        while (p < end) {
            uint32_t value = 0;
            unsigned shift = 0;
            // Decode one varint
            for (;;) {
                const uint8_t byte = *p++;
                value |= (byte & 0x7Fu) << shift;
                if ((byte & 0x80u) == 0) {
                    break;
                }
                shift += 7u;
            }
            sum += value;
        }
        total_sum = sum;
    }
    return total_sum;
}