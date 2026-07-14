#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t sum = 0;
    std::size_t n = deltas.size();
    const uint16_t* data = deltas.data();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t v = base + data[i];
            if (v >= low && v <= high) {
                s += v;
            }
        }
        sum = s;
    }
    return sum;
}