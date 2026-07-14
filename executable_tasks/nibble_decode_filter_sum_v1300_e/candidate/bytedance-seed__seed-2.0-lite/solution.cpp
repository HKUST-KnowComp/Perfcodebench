#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (uint8_t b : packed) {
            // Process low nibble first to match original order
            uint32_t low = static_cast<uint32_t>(b & 0x0Fu);
            if (low > threshold_u32) {
                sum += static_cast<uint64_t>(low);
            }
            // Process high nibble, matching original implementation
            uint32_t high = static_cast<uint32_t>(b >> 4u);
            if (high > threshold_u32) {
                sum += static_cast<uint64_t>(high);
            }
        }
    }
    return sum;
}