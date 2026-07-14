#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    uint64_t sum = 0;
    const uint32_t* const b = boundaries.data();
    const uint32_t* const p = probes.data();
    const size_t m = boundaries.size();
    const size_t n = probes.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t bucket = 0;
        for (size_t i = 0; i < n; ++i) {
            const uint32_t v = p[i];
            while (bucket < m && b[bucket] <= v) {
                ++bucket;
            }
            sum += bucket;
        }
    }
    return sum;
}