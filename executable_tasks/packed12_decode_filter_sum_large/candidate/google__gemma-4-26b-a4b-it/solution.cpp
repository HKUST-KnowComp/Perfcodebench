#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t i = 0;
        
        // Process in 3-byte blocks to extract two 12-bit values
        for (; i + 2 < n; i += 3) {
            // Value A: bits 0-7 from packed[i], bits 8-11 from packed[i+1] low nibble
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            
            // Value B: bits 0-3 from packed[i+1] high nibble, bits 4-11 from packed[i+2]
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4);

            if (a > thresh) current_iter_sum += a;
            if (b > thresh) current_iter_sum += b;
        }
        
        // Handle any trailing bytes if the input size is not a multiple of 3
        // (Though based on the logic, packed size should be a multiple of 3 for valid 12-bit packing)
        if (i < n) {
            // This part is technically unreachable if input is well-formed for 12-bit packing
            // but included for robustness.
            if (i + 1 < n) {
                uint32_t a = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i+1] & 0x0F) << 8);
                if (a > thresh) current_iter_sum += a;
            }
        }

        total_sum = current_iter_sum;
    }
    return total_sum;
}