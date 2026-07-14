#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (threshold >= 4096) {
        return 0;
    }
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < packed.size(); i += 3) {
            uint32_t a = packed[i] | (packed[i + 1] & 0x0F) << 8;
            uint32_t b = (packed[i + 1] >> 4) | (packed[i + 2] << 4);
            if (a > threshold) {
                sum += a;
            }
            if (b > threshold) {
                sum += b;
            }
        }
    }
    return sum;
}