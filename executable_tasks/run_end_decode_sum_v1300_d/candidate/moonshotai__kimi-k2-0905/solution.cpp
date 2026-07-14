#include "interface.h"
#include <cstddef>
#include <vector>

uint64_t run_end_sum(const std::vector<uint32_t>& run_ends,
                     const std::vector<uint32_t>& values,
                     int iters) {
    const std::size_t n = run_ends.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        uint32_t prev = 0;
        for (std::size_t i = 0; i != n; ++i) {
            const uint32_t next = run_ends[i];
            const uint32_t len = next - prev;   // run length (always > 0)
            sum += static_cast<uint64_t>(len) * static_cast<uint64_t>(values[i]);
            prev = next;
        }
        total = sum;         // store last iteration's result
    }
    return total;
}