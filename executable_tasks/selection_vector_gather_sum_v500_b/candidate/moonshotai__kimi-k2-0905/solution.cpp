#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters)
{
    const std::size_t n = keys.size();
    const uint32_t*  k = keys.data();
    const uint8_t*   f = flags.data();
    const uint32_t*  p = payloads.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        std::size_t i = 0;

        // peel remainder to keep aligned 4-wide unroll
        for (; i + 3 < n; i += 4) {
            uint32_t m0 = (f[i+0] && k[i+0] < threshold && ((k[i+0] & 7u) == 0u));
            uint32_t m1 = (f[i+1] && k[i+1] < threshold && ((k[i+1] & 7u) == 0u));
            uint32_t m2 = (f[i+2] && k[i+2] < threshold && ((k[i+2] & 7u) == 0u));
            uint32_t m3 = (f[i+3] && k[i+3] < threshold && ((k[i+3] & 7u) == 0u));

            s0 += m0 ? p[i+0] : 0;
            s1 += m1 ? p[i+1] : 0;
            s2 += m2 ? p[i+2] : 0;
            s3 += m3 ? p[i+3] : 0;
        }
        // tail
        for (; i < n; ++i) {
            if (f[i] && k[i] < threshold && ((k[i] & 7u) == 0u))
                s0 += p[i];
        }
        sum = s0 + s1 + s2 + s3;
    }
    return sum;
}