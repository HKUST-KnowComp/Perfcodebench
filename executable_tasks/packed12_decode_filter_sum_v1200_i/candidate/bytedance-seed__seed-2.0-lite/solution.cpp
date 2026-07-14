#include "interface.h"
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    const size_t total_bytes = packed.size();
    if (total_bytes == 0) return 0;
    const uint8_t* const data = packed.data();
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < total_bytes; i += 3) {
            const uint32_t a = static_cast<uint32_t>(data[i]) | (static_cast<uint32_t>(data[i+1] & 0x0F) << 8);
            const uint32_t b = (static_cast<uint32_t>(data[i+1] >> 4)) | (static_cast<uint32_t>(data[i+2]) << 4);
            if (a > thresh) sum += a;
            if (b > thresh) sum += b;
        }
        final_sum = sum;
    }
    return final_sum;
}