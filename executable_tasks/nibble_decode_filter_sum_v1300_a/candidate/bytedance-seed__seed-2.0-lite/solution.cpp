#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* __restrict__ pack_data = packed.data();
    const std::size_t pack_len = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < pack_len; ++i) {
            const uint8_t b = pack_data[i];
            // Process low nibble first, matching baseline order
            const uint32_t low_nib = b & 15u;
            if (low_nib > thresh) {
                current_sum += low_nib;
            }
            const uint32_t high_nib = b >> 4u;
            if (high_nib > thresh) {
                current_sum += high_nib;
            }
        }
        final_sum = current_sum;
    }
    return final_sum;
}