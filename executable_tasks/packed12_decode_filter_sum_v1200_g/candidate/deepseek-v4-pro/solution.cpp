#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t size = packed.size();
    for (size_t i = 0; i + 2 < size; i += 3) {
        uint32_t b0 = data[i];
        uint32_t b1 = data[i+1];
        uint32_t b2 = data[i+2];
        uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
        if (a > static_cast<uint32_t>(threshold)) sum += a;
        uint32_t b = (b1 >> 4u) | (b2 << 4u);
        if (b > static_cast<uint32_t>(threshold)) sum += b;
    }
    return sum;
}