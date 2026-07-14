#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = encoded.data();
    const size_t size = encoded.size();

    // The problem asks for the sum of all decoded values across all iterations.
    // However, the baseline resets 'sum = 0' inside the loop and returns the sum of the LAST iteration.
    // Re-reading baseline: 'sum = 0' is inside the 'iters' loop, but 'sum' is returned at the end.
    // This means the function actually returns the sum of the final iteration only.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        while (i < size) {
            uint32_t value = 0;
            uint32_t shift = 0;
            while (true) {
                uint8_t byte = data[i++];
                value |= static_cast<uint32_t>(byte & 0x7F) << shift;
                if (!(byte & 0x80)) break;
                shift += 7;
            }
            current_sum += value;
        }
        total_sum = current_sum;
    }
    return total_sum;
}