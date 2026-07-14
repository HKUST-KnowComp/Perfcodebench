#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    auto bit = boundaries.begin();
    const auto bend = boundaries.end();
    for (uint32_t p : probes) {
        while (bit != bend && *bit <= p) {
            ++bit;
        }
        sum += static_cast<uint64_t>(bit - boundaries.begin());
    }
    return sum;
}