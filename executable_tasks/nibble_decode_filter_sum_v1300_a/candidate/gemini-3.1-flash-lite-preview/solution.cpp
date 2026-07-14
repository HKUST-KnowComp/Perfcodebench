#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = data[i];
            uint32_t n1 = byte & 0x0Fu;
            uint32_t n2 = byte >> 4u;
            
            if (n1 > thresh) current_sum += n1;
            if (n2 > thresh) current_sum += n2;
        }
        total_sum = current_sum;
    }
    return total_sum;
}