#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thresh = static_cast<uint32_t>(threshold);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t i = 0;
        
        // Process in chunks of 3 bytes (which contain two 12-bit values)
        // Unrolling or SIMD could be applied, but a tight loop with fused logic
        // is the most significant improvement over the baseline.
        for (; i + 2 < n; i += 3) {
            // Decode first 12-bit value: [byte0: 8 bits][byte1: low 4 bits]
            uint32_t a = static_cast<uint32_t>(data[i]) | 
                         (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
            
            // Decode second 12-bit value: [byte1: high 4 bits][byte2: 8 bits]
            uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                         (static_cast<uint32_t>(data[i + 2]) << 4);

            if (a > thresh) current_sum += a;
            if (b > thresh) current_sum += b;
        }
        
        // Handle trailing bytes if the input size is not a multiple of 3
        // (Though the problem implies valid packed 12-bit data)
        if (i < n) {
            // This part is technically unreachable if input is perfectly packed
            // but included for robustness.
            if (i + 1 < n) {
                uint32_t a = static_cast<uint32_t>(data[i]) | 
                             (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
                if (a > thresh) current_sum += a;
            }
        }

        total_sum = current_sum;
    }

    return total_sum;
}