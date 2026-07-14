#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    if (iters <= 0) return 0;

    // Quick exits: no delta can produce a value in [low, high]
    if (base > high) return 0;
    if (base + 65535u < low) return 0;

    uint64_t sum = 0;
    const uint16_t* data = deltas.data();
    const std::size_t n = deltas.size();

    for (std::size_t i = 0; i < n; ++i) {
        const uint32_t val = base + static_cast<uint32_t>(data[i]);
        if (val >= low && val <= high) {
            sum += static_cast<uint64_t>(val);
        }
    }

    return sum;
}