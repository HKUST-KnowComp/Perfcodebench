#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
    uint64_t sum = 0;
    const std::size_t n = values.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cur_sum = 0;
        uint32_t prev = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t len = run_ends[i] - prev;
            cur_sum += static_cast<uint64_t>(len) * values[i];
            prev = run_ends[i];
        }
        sum = cur_sum;
    }
    return sum;
}
