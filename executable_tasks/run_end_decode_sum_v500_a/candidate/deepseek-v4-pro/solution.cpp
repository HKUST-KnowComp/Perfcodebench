#include "interface.h"

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int /* iters */) {
    uint64_t sum = 0;
    uint32_t prev = 0;
    const size_t n = run_ends.size();
    for (size_t i = 0; i < n; ++i) {
        const uint64_t len = static_cast<uint64_t>(run_ends[i]) - prev;
        sum += len * values[i];
        prev = run_ends[i];
    }
    return sum;
}