#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t size = encoded.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        const uint8_t* const end = data + size;
        uint64_t sum = 0;

        while (p < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            for (;;) {
                const uint8_t byte = *p++;
                value |= static_cast<uint32_t>(byte & 127u) << shift;
                if ((byte & 128u) == 0u) {
                    break;
                }
                shift += 7u;
            }
            sum += static_cast<uint64_t>(value);
        }
        total_sum = sum;
    }
    return total_sum;
}
