#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const size_t num_triplets = packed.size() / 3;
    const uint32_t thr = static_cast<uint32_t>(threshold);
    const uint8_t* const p = packed.data();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < num_triplets; ++i) {
            const size_t base = i * 3;
            const uint8_t b0 = p[base];
            const uint8_t b1 = p[base + 1];
            const uint8_t b2 = p[base + 2];

            const uint32_t a = b0 | (static_cast<uint32_t>(b1 & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(b1 >> 4)) | (static_cast<uint32_t>(b2) << 4);

            if (a > thr) sum += a;
            if (b > thr) sum += b;
        }
    }
    return sum;
}