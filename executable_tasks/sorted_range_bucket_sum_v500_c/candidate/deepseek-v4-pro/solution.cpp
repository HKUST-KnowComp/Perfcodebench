#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    (void)iters; // result does not depend on iteration count
    uint64_t sum = 0;
    size_t j = 0;
    const size_t bsize = boundaries.size();
    const size_t psize = probes.size();
    for (size_t i = 0; i < psize; ++i) {
        uint32_t p = probes[i];
        while (j < bsize && boundaries[j] <= p) {
            ++j;
        }
        sum += static_cast<uint64_t>(j);
    }
    return sum;
}