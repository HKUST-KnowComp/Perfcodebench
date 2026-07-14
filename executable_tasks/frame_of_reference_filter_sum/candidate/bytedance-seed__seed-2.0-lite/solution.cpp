#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t sum = 0;
    const size_t num_deltas = deltas.size();
    const uint16_t* delta_data = deltas.data();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < num_deltas; ++i) {
            const uint32_t value = base + static_cast<uint32_t>(delta_data[i]);
            if (value >= low && value <= high) {
                sum += static_cast<uint64_t>(value);
            }
        }
    }
    return sum;
}