#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t sum = 0;
    const uint32_t* data = deltas.data();
    const size_t n = deltas.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = base;
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            current += data[i];
            iter_sum += current;
        }
        sum = iter_sum;
    }
    return sum;
}