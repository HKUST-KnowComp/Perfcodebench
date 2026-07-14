#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t *pb = boundaries.data();
    const uint32_t *pe = pb + boundaries.size();
    const uint32_t *pp = probes.data();
    const uint32_t *ps = pp + probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint32_t *b = pb;
        const uint32_t *p = pp;
        uint64_t bucket = 0;
        while (p != ps) {
            while (b != pe && *b <= *p) { ++b; ++bucket; }
            sum += bucket;
            ++p;
        }
    }
    return sum;
}