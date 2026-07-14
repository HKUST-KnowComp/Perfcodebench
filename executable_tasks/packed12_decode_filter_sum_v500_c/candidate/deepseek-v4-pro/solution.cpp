#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;
    const uint8_t* data = packed.data();
    const std::size_t size = packed.size();
    uint64_t single_sum = 0;
    const uint32_t thresh = static_cast<uint32_t>(threshold);
    
    for (std::size_t i = 0; i + 2 < size; i += 3) {
        const uint32_t a = static_cast<uint32_t>(data[i]) |
                          ((static_cast<uint32_t>(data[i + 1]) & 0x0Fu) << 8);
        const uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4) |
                          (static_cast<uint32_t>(data[i + 2]) << 4);
        if (a > thresh) single_sum += a;
        if (b > thresh) single_sum += b;
    }
    
    return single_sum * static_cast<uint64_t>(iters);
}
