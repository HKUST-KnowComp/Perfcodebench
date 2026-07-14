#include "interface.h"
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    size_t size = encoded.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* ptr = data;
        const uint8_t* end = data + size;
        uint64_t local_sum = 0;
        while (ptr < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            while (true) {
                uint8_t byte = *ptr++;
                value |= static_cast<uint32_t>(byte & 0x7fu) << shift;
                if ((byte & 0x80u) == 0u) break;
                shift += 7u;
            }
            local_sum += value;
        }
        sum = local_sum;
    }
    return sum;
}