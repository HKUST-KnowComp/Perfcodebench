#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    (void)iters;
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    size_t size = packed.size();
    for (size_t i = 0; i + 2 < size; i += 3) {
        uint32_t a = data[i] | ((data[i+1] & 0x0F) << 8);
        uint32_t b = (data[i+1] >> 4) | (data[i+2] << 4);
        if (a > threshold) sum += a;
        if (b > threshold) sum += b;
    }
    return sum;
}