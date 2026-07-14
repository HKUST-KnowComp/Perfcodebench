#include "interface.h"
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const size_t sz = packed.size();
    const uint8_t* data = packed.data();
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    for (size_t i = 0; i + 3 <= sz; i += 3) {
        uint32_t val0 = data[i] | ((data[i+1] & 0x0F) << 8);
        if (val0 > thresh) sum += val0;
        uint32_t val1 = (data[i+1] >> 4) | (data[i+2] << 4);
        if (val1 > thresh) sum += val1;
    }
    return sum;
}