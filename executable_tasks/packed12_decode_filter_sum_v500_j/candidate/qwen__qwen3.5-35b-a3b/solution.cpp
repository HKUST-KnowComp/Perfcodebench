#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint32_t thresh = static_cast<uint32_t>(threshold);
        sum = 0;
        for (std::size_t i = 0; i < packed.size(); i += 3) {
            uint32_t a = packed[i] | (static_cast<uint32_t>(packed[i + 1]) & 0x0F) << 8;
            uint32_t b = (static_cast<uint32_t>(packed[i + 1]) >> 4) | (static_cast<uint32_t>(packed[i + 2]) << 4);
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
        }
    }
    return sum;
}