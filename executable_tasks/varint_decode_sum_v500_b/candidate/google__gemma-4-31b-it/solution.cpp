#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    if (encoded.empty()) return 0;
    
    const uint8_t* data = encoded.data();
    const uint8_t* end = data + encoded.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        const uint8_t* ptr = data;
        
        while (ptr < end) {
            uint32_t value = 0;
            uint32_t shift = 0;
            
            while (true) {
                uint8_t byte = *ptr++;
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