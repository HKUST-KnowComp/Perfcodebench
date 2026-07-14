#include "interface.h"
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    size_t size = encoded.size();
    uint64_t sum = 0;
    for (int i = 0; i < iters; ++i) {
        const uint8_t* ptr = data;
        const uint8_t* end = ptr + size;
        sum = 0;
        while (ptr < end) {
            uint64_t val = 0;
            uint64_t shift = 0;
            while (true) {
                uint8_t byte = *ptr++;
                val |= static_cast<uint64_t>(byte & 0x7F) << shift;
                if (!(byte & 0x80)) break;
                shift += 7;
            }
            sum += val;
        }
    }
    return sum;
}