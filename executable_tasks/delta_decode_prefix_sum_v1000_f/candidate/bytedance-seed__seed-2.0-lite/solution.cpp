#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current = base;
        uint64_t sum = 0;
        for (uint32_t d : deltas) {
            current += d;
            sum += current;
        }
        final_sum = sum;
    }
    return final_sum;
}