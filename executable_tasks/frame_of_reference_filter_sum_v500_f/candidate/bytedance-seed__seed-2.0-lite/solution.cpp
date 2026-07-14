#include "interface.h"
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    const size_t n = deltas.size();
    const uint16_t* d_ptr = deltas.data();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            const uint32_t val = base + static_cast<uint32_t>(d_ptr[i]);
            if (val >= low && val <= high) {
                sum += static_cast<uint64_t>(val);
            }
        }
    }
    return sum;
}