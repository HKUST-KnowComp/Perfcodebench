#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* src = packed.data();
    const uint8_t* end = src + packed.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        src = packed.data();
        end = src + packed.size();
        while (src < end) {
            uint32_t a = static_cast<uint32_t>(src[0]) | ((static_cast<uint32_t>(src[1] & 0x0Fu)) << 8u);
            uint32_t b = (static_cast<uint32_t>(src[1] >> 4u)) | (static_cast<uint32_t>(src[2]) << 4u);
            if (a > static_cast<uint32_t>(threshold)) sum += a;
            if (b > static_cast<uint32_t>(threshold)) sum += b;
            src += 3;
        }
    }
    return sum;
}
