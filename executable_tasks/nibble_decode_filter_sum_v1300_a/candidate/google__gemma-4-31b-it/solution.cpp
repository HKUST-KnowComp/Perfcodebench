#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Unroll loop to process multiple bytes at once
        for (; i + 3 < size; i += 4) {
            uint8_t b0 = data[i];
            uint8_t b1 = data[i+1];
            uint8_t b2 = data[i+2];
            uint8_t b3 = data[i+3];

            uint8_t n0 = b0 & 0x0F;
            uint8_t n1 = b0 >> 4;
            uint8_t n2 = b1 & 0x0F;
            uint8_t n3 = b1 >> 4;
            uint8_t n4 = b2 & 0x0F;
            uint8_t n5 = b2 >> 4;
            uint8_t n6 = b3 & 0x0F;
            uint8_t n7 = b3 >> 4;

            if (n0 > threshold) current_sum += n0;
            if (n1 > threshold) current_sum += n1;
            if (n2 > threshold) current_sum += n2;
            if (n3 > threshold) current_sum += n3;
            if (n4 > threshold) current_sum += n4;
            if (n5 > threshold) current_sum += n5;
            if (n6 > threshold) current_sum += n6;
            if (n7 > threshold) current_sum += n7;
        }

        // Handle remaining bytes
        for (; i < size; ++i) {
            uint8_t b = data[i];
            uint8_t n_low = b & 0x0F;
            uint8_t n_high = b >> 4;
            if (n_low > threshold) current_sum += n_low;
            if (n_high > threshold) current_sum += n_high;
        }
        
        total_sum = current_sum;
    }
    
    return total_sum;
}