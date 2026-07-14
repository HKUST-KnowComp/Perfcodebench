#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    size_t size = packed.size();
    uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = data[i];
            
            // Extract low nibble
            uint8_t low = byte & 0x0F;
            if (low > threshold) {
                current_sum += low;
            }
            
            // Extract high nibble
            uint8_t high = byte >> 4;
            if (high > threshold) {
                current_sum += high;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}