#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t total_packed_bytes = packed.size();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        final_sum = 0;
        for (std::size_t i = 0; i < total_packed_bytes; i += 3) {
            // Exact same decoding logic as baseline to preserve correctness
            const uint32_t a = static_cast<uint32_t>(packed[i]) | 
                               (static_cast<uint32_t>(packed[i + 1] & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(packed[i + 1] >> 4)) | 
                               (static_cast<uint32_t>(packed[i + 2]) << 4);
            
            // Fused filter and sum to eliminate intermediate storage
            if (a > thresh) final_sum += a;
            if (b > thresh) final_sum += b;
        }
    }

    return final_sum;
}