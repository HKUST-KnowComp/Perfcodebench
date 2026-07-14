#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(const std::vector<uint16_t>& deltas, uint32_t base, uint32_t low, uint32_t high, int iters) {
    (void)iters;
    uint64_t sum = 0;
    const uint16_t* data = deltas.data();
    const std::size_t n = deltas.size();
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t val = base + data[i];
        if (val >= low && val <= high) {
            sum += val;
        }
    }
    return sum;
}