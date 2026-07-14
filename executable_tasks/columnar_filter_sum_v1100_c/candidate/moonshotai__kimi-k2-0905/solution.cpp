#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters)
{
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    const uint8_t*  f = flags.data();
    const std::size_t n = keys.size();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
        std::size_t i = 0;

        // 8-way unrolled main loop
        for (; i + 8 <= n; i += 8) {
            const uint32_t k0 = k[i+0], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
            const uint32_t k4 = k[i+4], k5 = k[i+5], k6 = k[i+6], k7 = k[i+7];

            const bool c0 = (f[i+0] && k0 < key_threshold && ((k0 & 3u) == 0u));
            const bool c1 = (f[i+1] && k1 < key_threshold && ((k1 & 3u) == 0u));
            const bool c2 = (f[i+2] && k2 < key_threshold && ((k2 & 3u) == 0u));
            const bool c3 = (f[i+3] && k3 < key_threshold && ((k3 & 3u) == 0u));
            const bool c4 = (f[i+4] && k4 < key_threshold && ((k4 & 3u) == 0u));
            const bool c5 = (f[i+5] && k5 < key_threshold && ((k5 & 3u) == 0u));
            const bool c6 = (f[i+6] && k6 < key_threshold && ((k6 & 3u) == 0u));
            const bool c7 = (f[i+7] && k7 < key_threshold && ((k7 & 3u) == 0u));

            s0 += c0 ? v[i+0] : 0;
            s1 += c1 ? v[i+1] : 0;
            s2 += c2 ? v[i+2] : 0;
            s3 += c3 ? v[i+3] : 0;
            s4 += c4 ? v[i+4] : 0;
            s5 += c5 ? v[i+5] : 0;
            s6 += c6 ? v[i+6] : 0;
            s7 += c7 ? v[i+7] : 0;
        }

        // scalar tail
        uint64_t tail = 0;
        for (; i < n; ++i) {
            const uint32_t key = k[i];
            if (f[i] && key < key_threshold && ((key & 3u) == 0u)) {
                tail += v[i];
            }
        }

        total = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + tail;
    }

    return total;
}