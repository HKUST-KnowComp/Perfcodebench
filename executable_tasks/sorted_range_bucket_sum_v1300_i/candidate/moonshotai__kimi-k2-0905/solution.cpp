#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t * const pb = boundaries.data();
    const uint32_t * const pp = probes.data();
    const size_t nb = boundaries.size();
    const size_t np = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t b = 0;
        const size_t np4 = np & ~size_t(3);
        size_t p = 0;

        // unrolled 4× to hide latency and reduce loop overhead
        for (; p < np4; p += 4) {
            uint32_t v0 = pp[p+0];
            uint32_t v1 = pp[p+1];
            uint32_t v2 = pp[p+2];
            uint32_t v3 = pp[p+3];

            while (b < nb && pb[b] <= v0) ++b;
            sum += b;
            const size_t b0 = b;

            while (b < nb && pb[b] <= v1) ++b;
            sum += b;
            const size_t b1 = b;

            while (b < nb && pb[b] <= v2) ++b;
            sum += b;
            const size_t b2 = b;

            while (b < nb && pb[b] <= v3) ++b;
            sum += b;
        }
        // tail
        for (; p < np; ++p) {
            uint32_t v = pp[p];
            while (b < nb && pb[b] <= v) ++b;
            sum += b;
        }
    }
    return sum;
}