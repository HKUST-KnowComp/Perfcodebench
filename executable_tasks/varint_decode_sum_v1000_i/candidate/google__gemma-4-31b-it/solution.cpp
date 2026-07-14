#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        while (i < size) {
            uint32_t value = 0;
            uint32_t shift = 0;
            while (true) {
                uint8_t byte = data[i++];
                value |= static_cast<uint32_t>(byte & 0x7F) << shift;
                if (!(byte & 0x80)) {
                    break;
                }
                shift += 7;
            }
            current_sum += value;
        }
        total_sum = current_sum;
    }
    return total_sum;
}