#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const std::size_t packed_len = packed.size();
    const uint8_t* const packed_data = packed.data();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < packed_len; ++i) {
            const uint8_t byte = packed_data[i];
            const uint8_t low_nibble = byte & 0xFU;
            if (low_nibble > threshold) {
                sum += low_nibble;
            }
            const uint8_t high_nibble = byte >> 4U;
            if (high_nibble > threshold) {
                sum += high_nibble;
            }
        }
    }
    return sum;
}