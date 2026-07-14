#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    size_t n = packed.size();
    size_t count = n / 3;
    uint64_t sum = 0;
    uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t accum = 0;
        const uint8_t* ptr = data;
        size_t remaining = count;

        while (remaining >= 2) {
            uint32_t w0 = static_cast<uint32_t>(ptr[0]) | (static_cast<uint32_t>(ptr[1] & 0x0F) << 8);
            uint32_t w1 = (static_cast<uint32_t>(ptr[1] >> 4)) | (static_cast<uint32_t>(ptr[2]) << 4);
            uint32_t w2 = static_cast<uint32_t>(ptr[3]) | (static_cast<uint32_t>(ptr[4] & 0x0F) << 8);
            uint32_t w3 = (static_cast<uint32_t>(ptr[4] >> 4)) | (static_cast<uint32_t>(ptr[5]) << 4);

            accum += (w0 > thresh) ? w0 : 0;
            accum += (w1 > thresh) ? w1 : 0;
            accum += (w2 > thresh) ? w2 : 0;
            accum += (w3 > thresh) ? w3 : 0;

            ptr += 6;
            remaining -= 2;
        }

        while (remaining > 0) {
            uint32_t a = static_cast<uint32_t>(ptr[0]) | (static_cast<uint32_t>(ptr[1] & 0x0F) << 8);
            uint32_t b = (static_cast<uint32_t>(ptr[1] >> 4)) | (static_cast<uint32_t>(ptr[2]) << 4);
            accum += (a > thresh) ? a : 0;
            accum += (b > thresh) ? b : 0;
            ptr += 3;
            remaining -= 1;
        }

        sum = accum;
    }
    return sum;
}