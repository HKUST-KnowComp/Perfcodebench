#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t result = 0;
    const uint32_t* data = deltas.data();
    const size_t n = deltas.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        uint64_t value = base;
        for (size_t i = 0; i < n; ++i) {
            value += data[i];
            current_sum += value;
        }
        result = current_sum;
    }
    return result;
}