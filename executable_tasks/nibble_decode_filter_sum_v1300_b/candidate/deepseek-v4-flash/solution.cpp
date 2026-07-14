#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    uint32_t thresh = static_cast<uint32_t>(threshold);
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (const uint8_t* p = data, *end = data + n; p != end; ++p) {
            uint8_t byte = *p;
            uint32_t low = byte & 0x0Fu;
            uint32_t high = byte >> 4u;
            if (low > thresh) sum += low;
            if (high > thresh) sum += high;
        }
    }
    return sum;
}
