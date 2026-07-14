#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    if (iters <= 0) return 0;
    const auto* data = packed.data();
    const std::size_t size = packed.size();
    uint64_t sum = 0;
    const uint32_t th = static_cast<uint32_t>(threshold);
    for (std::size_t i = 0; i < size; i += 3) {
        // Decode two 12-bit values from three bytes
        uint32_t a = static_cast<uint32_t>(data[i]) |
                     (static_cast<uint32_t>(data[i + 1] & 0x0F) << 8);
        uint32_t b = (static_cast<uint32_t>(data[i + 1] >> 4)) |
                     (static_cast<uint32_t>(data[i + 2]) << 4);
        // Branchless addition: add value if greater than threshold
        sum += a & -(a > th);
        sum += b & -(b > th);
    }
    return sum;
}
