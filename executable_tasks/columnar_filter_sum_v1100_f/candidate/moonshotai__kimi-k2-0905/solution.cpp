#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters)
{
    const std::size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t*  f = flags.data();

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::size_t i = 0;
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

        // 4-way unrolled main loop
        for (; i + 3 < n; i += 4) {
            const uint32_t k0 = k[i];
            const uint32_t k1 = k[i+1];
            const uint32_t k2 = k[i+2];
            const uint32_t k3 = k[i+3];

            const uint8_t  m0 = f[i];
            const uint8_t  m1 = f[i+1];
            const uint8_t  m2 = f[i+2];
            const uint8_t  m3 = f[i+3];

            const uint32_t c0 = (k0 < key_threshold) & ((k0 & 3u) == 0u);
            const uint32_t c1 = (k1 < key_threshold) & ((k1 & 3u) == 0u);
            const uint32_t c2 = (k2 < key_threshold) & ((k2 & 3u) == 0u);
            const uint32_t c3 = (k3 < key_threshold) & ((k3 & 3u) == 0u);

            s0 += (m0 & c0) ? v[i]   : 0;
            s1 += (m1 & c1) ? v[i+1] : 0;
            s2 += (m2 & c2) ? v[i+2] : 0;
            s3 += (m3 & c3) ? v[i+3] : 0;
        }

        // scalar tail
        for (; i < n; ++i) {
            if (f[i] && k[i] < key_threshold && ((k[i] & 3u) == 0u)) {
                s0 += v[i];
            }
        }
        total = s0 + s1 + s2 + s3;
    }
    return total;
}