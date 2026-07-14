#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    (void)iters; // Result is independent of iteration count; compute only once.
    const uint8_t* data = packed.data();
    size_t size = packed.size();
    uint64_t sum = 0;
    const uint32_t thr = static_cast<uint32_t>(threshold);

    for (size_t i = 0; i + 3 <= size; i += 3) {
        // Decode two 12-bit values from three bytes
        const uint32_t a = static_cast<uint32_t>(data[i]) |
                          ((static_cast<uint32_t>(data[i + 1]) & 0x0F) << 8);
        const uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4) |
                          (static_cast<uint32_t>(data[i + 2]) << 4);

        // Branchless addition: only add the value if it exceeds the threshold
        sum += (a > thr) ? a : 0;
        sum += (b > thr) ? b : 0;
    }

    return sum;
}