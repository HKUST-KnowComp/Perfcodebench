
#include "interface.h"
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {

    uint64_t sum = 0u;

    // manually unroll the outer iteration a bit to shrink front->back branch mispredictions
    const int iters4 = iters & ~3;

    for (int outer = 0; outer < iters4; outer += 4) {
        for (int rep = 0; rep < 4; ++rep) {
            uint64_t acc = 0u;
            uint32_t prev = 0u;
            const size_t n = values.size();
            for (size_t i = 0; i < n; ++i) {
                const uint32_t len = run_ends[i] - prev;
                acc += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
                prev = run_ends[i];
            }
            sum = acc;  // final iteration of the unrolled block hits the only visible side effect
        }
    }

    // tail
    for (int outer = iters4; outer < iters; ++outer) {
        uint64_t acc = 0u;
        uint32_t prev = 0u;
        const size_t n = values.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t len = run_ends[i] - prev;
            acc += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
            prev = run_ends[i];
        }
        sum = acc;
    }

    return sum;
}