#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    if (n == 0 || iters == 0) return 0;

    uint64_t total = 0;
    const uint32_t* __restrict d = deltas.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc = 0;
        uint64_t value = base;

        std::size_t i = 0;
        // 4× unrolled inner loop
        for (; i + 4 <= n; i += 4) {
            value += d[i + 0]; acc += value;
            value += d[i + 1]; acc += value;
            value += d[i + 2]; acc += value;
            value += d[i + 3]; acc += value;
        }
        // handle tail
        for (; i < n; ++i) {
            value += d[i];
            acc += value;
        }
        total = acc;
    }
    return total;
}