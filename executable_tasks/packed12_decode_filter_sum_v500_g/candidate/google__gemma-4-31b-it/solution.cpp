#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; i += 3) {
            // Decode first 12-bit value: [i] (8 bits) | [i+1] low 4 bits
            uint32_t a = data[i] | ((data[i + 1] & 0x0F) << 8);
            // Decode second 12-bit value: [i+1] high 4 bits | [i+2] (8 bits)
            uint32_t b = (data[i + 1] >> 4) | (data[i + 2] << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        total_sum = current_sum;
    }
    return total_sum;
}