#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint8_t* data = packed.data();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t byte = data[i];
            uint32_t low = byte & 0x0Fu;
            uint32_t high = byte >> 4u;

            if (low > thresh) {
                current_iter_sum += low;
            }
            if (high > thresh) {
                current_iter_sum += high;
            }
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}