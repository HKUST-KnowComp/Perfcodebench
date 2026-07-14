#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    uint64_t sum = 0;
    auto it = boundaries.begin();
    const auto end = boundaries.end();
    for (uint32_t p : probes) {
        while (it != end && *it <= p) {
            ++it;
        }
        sum += static_cast<uint64_t>(it - boundaries.begin());
    }
    return sum;
}