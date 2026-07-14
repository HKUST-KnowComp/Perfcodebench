#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const size_t n = packed.size();
    const uint8_t* data = packed.data();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        size_t i = 0;
        
        // Process 8 bytes at a time (16 nibbles) to improve ILP
        for (; i + 7 < n; i += 8) {
            for (size_t j = 0; j < 8; ++j) {
                uint8_t byte = data[i + j];
                uint32_t low = byte & 0x0F;
                uint32_t high = byte >> 4;
                if (low > thresh) iter_sum += low;
                if (high > thresh) iter_sum += high;
            }
        }
        
        // Handle remaining bytes
        for (; i < n; ++i) {
            uint8_t byte = data[i];
            uint32_t low = byte & 0x0F;
            uint32_t high = byte >> 4;
            if (low > thresh) iter_sum += low;
            if (high > thresh) iter_sum += high;
        }
        total_sum = iter_sum;
    }
    return total_sum;
}