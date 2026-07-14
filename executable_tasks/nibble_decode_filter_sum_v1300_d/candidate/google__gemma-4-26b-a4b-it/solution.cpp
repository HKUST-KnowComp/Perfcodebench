#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = data[i];
            
            // Extract low nibble
            uint32_t low = static_cast<uint32_t>(byte & 0x0F);
            if (low > thresh) {
                current_iter_sum += low;
            }
            
            // Extract high nibble
            uint32_t high = static_cast<uint32_t>(byte >> 4);
            if (high > thresh) {
                current_iter_sum += high;
            }
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}