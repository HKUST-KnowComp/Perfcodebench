#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t sum = 0;
    const uint32_t* __restrict__ delta_data = deltas.data();
    const std::size_t delta_count = deltas.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_value = base;
        sum = 0;
        for (std::size_t i = 0; i < delta_count; ++i) {
            current_value += static_cast<uint64_t>(delta_data[i]);
            sum += current_value;
        }
    }
    return sum;
}