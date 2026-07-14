#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const std::size_t total_full_chunks = packed.size() / 3;
    const std::size_t max_i = total_full_chunks * 3;
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < max_i; i += 3) {
            const uint32_t a = static_cast<uint32_t>(packed[i]) | (static_cast<uint32_t>(packed[i+1] & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(packed[i+1] >> 4)) | (static_cast<uint32_t>(packed[i+2]) << 4);
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
        }
    }
    return sum;
}