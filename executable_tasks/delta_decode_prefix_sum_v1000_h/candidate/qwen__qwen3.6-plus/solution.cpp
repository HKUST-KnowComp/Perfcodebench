#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t result = 0;
    const size_t n = deltas.size();
    const uint32_t* data = deltas.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = base;
        uint64_t s = 0;
        for (size_t i = 0; i < n; ++i) {
            current += data[i];
            s += current;
        }
        result = s;
    }
    return result;
}