#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(const std::vector<uint16_t>& deltas,
                          uint32_t base,
                          uint32_t low,
                          uint32_t high,
                          int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        const uint16_t* __restrict d = deltas.data();
        const std::size_t n = deltas.size();

        for (std::size_t i = 0; i < n; ++i) {
            uint32_t v = base + static_cast<uint32_t>(d[i]);
            if (v >= low && v <= high) {
                sum += static_cast<uint64_t>(v);
            }
        }
        total = sum;
    }
    return total;
}