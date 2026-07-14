#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Process in blocks of 9 bytes to decode 6 values per block
        for (; i + 8 < size; i += 9) {
            const uint8_t* p = &data[i];
            
            // Value 1
            uint32_t v1 = p[0] | ((p[1] & 0x0F) << 8);
            if (v1 > thresh) current_sum += v1;
            
            // Value 2
            uint32_t v2 = (p[1] >> 4) | (p[2] << 4);
            if (v2 > thresh) current_sum += v2;
            
            // Value 3
            uint32_t v3 = p[3] | ((p[4] & 0x0F) << 8);
            if (v3 > thresh) current_sum += v3;
            
            // Value 4
            uint32_t v4 = (p[4] >> 4) | (p[5] << 4);
            if (v4 > thresh) current_sum += v4;
            
            // Value 5
            uint32_t v5 = p[6] | ((p[7] & 0x0F) << 8);
            if (v5 > thresh) current_sum += v5;
            
            // Value 6
            uint32_t v6 = (p[7] >> 4) | (p[8] << 4);
            if (v6 > thresh) current_sum += v6;
        }

        // Handle remaining bytes
        for (; i < size; i += 3) {
            if (i + 2 >= size) break;
            uint32_t v1 = data[i] | ((data[i + 1] & 0x0F) << 8);
            if (v1 > thresh) current_sum += v1;
            uint32_t v2 = (data[i + 1] >> 4) | (data[i + 2] << 4);
            if (v2 > thresh) current_sum += v2;
        }
        total_sum = current_sum;
    }
    return total_sum;
}