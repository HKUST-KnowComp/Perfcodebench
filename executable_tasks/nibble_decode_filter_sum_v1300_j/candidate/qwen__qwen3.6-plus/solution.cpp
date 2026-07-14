#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t lut[256];
    for (int i = 0; i < 256; ++i) {
        uint8_t lo = i & 0xF;
        uint8_t hi = i >> 4;
        lut[i] = (lo > threshold ? lo : 0) + (hi > threshold ? hi : 0);
    }

    uint64_t pass_sum = 0;
    for (uint8_t b : packed) {
        pass_sum += lut[b];
    }

    return pass_sum * static_cast<uint64_t>(iters);
}
