#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t total_sum = 0;
    const uint16_t* data = deltas.data();
    const size_t size = deltas.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint32_t value = base + data[i];
            if (value >= low && value <= high) {
                current_sum += value;
            }
        }
        total_sum = current_sum;
    }
    return total_sum;
}