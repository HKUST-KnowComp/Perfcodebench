#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint8_t thresh = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = data[i];
            
            // Low nibble
            uint8_t low = byte & 0x0F;
            if (low > thresh) {
                current_sum += low;
            }
            
            // High nibble
            uint8_t high = byte >> 4;
            if (high > thresh) {
                current_sum += high;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}