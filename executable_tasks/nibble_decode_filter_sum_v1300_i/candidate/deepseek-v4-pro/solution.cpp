#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint8_t filter[16];
    for (int v = 0; v < 16; ++v) {
        filter[v] = static_cast<uint8_t>(v > threshold ? v : 0);
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < packed.size(); ++i) {
            uint8_t byte = packed[i];
            sum += filter[byte & 0x0F];
            sum += filter[byte >> 4];
        }
    }
    return sum;
}
