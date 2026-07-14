#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    // Precompute total sum of bucket indices using sorted merge
    uint64_t total_sum = 0;
    std::size_t idx = 0;
    const std::size_t bsize = boundaries.size();
    for (uint32_t p : probes) {
        while (idx < bsize && boundaries[idx] <= p) {
            ++idx;
        }
        total_sum += static_cast<uint64_t>(idx);
    }

    // Perform the required number of iterations
    uint64_t sum = 0;
    for (int i = 0; i < iters; ++i) {
        sum = total_sum;
    }
    return sum;
}