#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t*  b = boundaries.data();
    const uint32_t*  p = probes.data();
    const size_t     B = boundaries.size();
    const size_t     P = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t bucket = 0;
        for (size_t i = 0; i < P; ++i) {
            const uint32_t v = p[i];
            while (bucket < B && b[bucket] <= v) ++bucket;
            sum += bucket;
        }
    }
    return sum;
}