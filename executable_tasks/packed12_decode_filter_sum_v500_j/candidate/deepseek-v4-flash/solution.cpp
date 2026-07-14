#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const uint8_t* end = data + packed.size();
    while (data < end) {
        uint32_t a = data[0] | ((data[1] & 0x0F) << 8);
        uint32_t b = (data[1] >> 4) | (data[2] << 4);
        if (a > threshold) sum += a;
        if (b > threshold) sum += b;
        data += 3;
    }
    return sum;
}
