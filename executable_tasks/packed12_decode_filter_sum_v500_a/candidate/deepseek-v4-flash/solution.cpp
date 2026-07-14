#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    const uint32_t thr = threshold;
    size_t i = 0;
    while (i + 2 < n) {
        uint32_t a = data[i] | ((uint32_t)(data[i+1] & 0x0F) << 8);
        uint32_t b = ((uint32_t)(data[i+1] >> 4)) | ((uint32_t)(data[i+2]) << 4);
        if (a > thr) sum += a;
        if (b > thr) sum += b;
        i += 3;
    }
    return sum;
}
