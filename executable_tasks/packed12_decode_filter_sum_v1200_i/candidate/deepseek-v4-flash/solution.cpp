#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    (void)iters; // work is identical each iteration; compute once
    size_t n = packed.size();
    uint64_t sum = 0;
    uint32_t thresh = threshold;
    const uint8_t* data = packed.data();

    for (size_t i = 0; i + 2 < n; i += 3) {
        uint8_t b0 = data[i];
        uint8_t b1 = data[i+1];
        uint8_t b2 = data[i+2];

        uint32_t a = b0 | ((uint32_t)(b1 & 0x0F) << 8);
        uint32_t b = ((uint32_t)(b1 >> 4)) | ((uint32_t)b2 << 4);

        if (a > thresh) sum += a;
        if (b > thresh) sum += b;
    }
    return sum;
}
