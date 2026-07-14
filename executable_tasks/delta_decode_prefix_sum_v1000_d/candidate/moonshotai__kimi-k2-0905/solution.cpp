#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    if (n == 0 || iters == 0) return 0;

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = base;
        uint64_t sum   = 0;

        std::size_t i = 0;
        // Process 4 elements per iteration
        for (; i + 3 < n; i += 4) {
            value += static_cast<uint64_t>(deltas[i]);
            sum   += value;

            value += static_cast<uint64_t>(deltas[i + 1]);
            sum   += value;

            value += static_cast<uint64_t>(deltas[i + 2]);
            sum   += value;

            value += static_cast<uint64_t>(deltas[i + 3]);
            sum   += value;
        }
        // Remainder
        for (; i < n; ++i) {
            value += static_cast<uint64_t>(deltas[i]);
            sum   += value;
        }
        total += sum;
    }
    return total;
}