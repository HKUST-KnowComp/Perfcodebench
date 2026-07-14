#include "interface.h"
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const std::size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;

        // Process 2 groups (6 bytes) at a time for unrolling
        for (; i + 6 <= size; i += 6) {
            uint32_t a0 = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t b0 = (data[i+1] >> 4) | (data[i+2] << 4);
            uint32_t a1 = data[i+3] | ((data[i+4] & 0x0F) << 8);
            uint32_t b1 = (data[i+4] >> 4) | (data[i+5] << 4);

            if (a0 > thresh) local_sum += a0;
            if (b0 > thresh) local_sum += b0;
            if (a1 > thresh) local_sum += a1;
            if (b1 > thresh) local_sum += b1;
        }

        // Process remaining complete groups (3 bytes each)
        for (; i + 3 <= size; i += 3) {
            uint32_t a = data[i] | ((data[i+1] & 0x0F) << 8);
            uint32_t b = (data[i+1] >> 4) | (data[i+2] << 4);

            if (a > thresh) local_sum += a;
            if (b > thresh) local_sum += b;
        }

        sum = local_sum;
    }

    return sum;
}
