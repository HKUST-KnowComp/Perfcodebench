#include "interface.h"

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t result = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = static_cast<uint64_t>(base);
        uint64_t sum = 0;
        for (auto d : deltas) {
            value += d;
            sum += value;
        }
        result = sum;
    }
    return result;
}