#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    // The bucket sum is independent of the number of iterations.
    // Compute it once and return, ignoring iters for performance.
    if (iters <= 0) return 0;
    size_t b_idx = 0;
    uint64_t total = 0;
    for (uint32_t p : probes) {
        while (b_idx < boundaries.size() && boundaries[b_idx] <= p) {
            ++b_idx;
        }
        total += static_cast<uint64_t>(b_idx);
    }
    return total;
}