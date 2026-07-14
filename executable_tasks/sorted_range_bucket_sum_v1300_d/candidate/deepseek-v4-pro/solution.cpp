#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    if (iters <= 0) return 0;

    uint64_t sum = 0;
    std::size_t idx = 0;
    const std::size_t bsize = boundaries.size();

    for (uint32_t probe : probes) {
        while (idx < bsize && boundaries[idx] <= probe) {
            ++idx;
        }
        sum += static_cast<uint64_t>(idx);
    }

    return sum;
}
