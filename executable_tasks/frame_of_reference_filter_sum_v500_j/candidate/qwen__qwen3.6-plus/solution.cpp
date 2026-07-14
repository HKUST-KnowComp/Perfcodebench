#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t sum = 0;
    const uint16_t* d_ptr = deltas.data();
    const size_t n = deltas.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t val = base + d_ptr[i];
            if (val >= low && val <= high) {
                iter_sum += val;
            }
        }
        sum = iter_sum;
    }
    return sum;
}
