#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        uint32_t prev = 0;
        const size_t n = values.size();
        for (size_t i = 0; i < n; ++i) {
            uint64_t run_len = run_ends[i] - prev;
            iter_sum += run_len * static_cast<uint64_t>(values[i]);
            prev = run_ends[i];
        }
        sum = iter_sum;
    }
    return sum;
}
