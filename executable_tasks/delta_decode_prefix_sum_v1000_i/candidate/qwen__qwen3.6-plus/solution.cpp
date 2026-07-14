#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const uint64_t n = deltas.size();
    const uint32_t* data = deltas.data();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = n * base;
        uint64_t weight = n;
        for (uint64_t i = 0; i < n; ++i) {
            current += (uint64_t)data[i] * weight;
            --weight;
        }
        sum = current;
    }
    return sum;
}