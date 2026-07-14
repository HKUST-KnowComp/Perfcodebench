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
    uint64_t sum = 0;
    const uint16_t* ptr = deltas.data();
    std::size_t n = deltas.size();
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t v = base + ptr[i];
        if (v >= low && v <= high) {
            sum += v;
        }
    }
    return sum;
}
