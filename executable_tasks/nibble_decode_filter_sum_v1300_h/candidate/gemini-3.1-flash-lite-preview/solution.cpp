#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint8_t* data = packed.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t byte = data[i];
            uint8_t low = byte & 0x0Fu;
            uint8_t high = byte >> 4u;

            if (low > threshold) {
                current_sum += low;
            }
            if (high > threshold) {
                current_sum += high;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}