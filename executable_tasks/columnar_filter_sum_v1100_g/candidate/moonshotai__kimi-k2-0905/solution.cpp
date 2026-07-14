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
    const size_t n = keys.size();
    const uint32_t* const k = keys.data();
    const uint32_t* const v = values.data();
    const uint8_t*  const f = flags.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        size_t i = 0;
        const size_t n4 = n & ~size_t(3);
        for (; i < n4; i += 4) {
            const uint32_t k0 = k[i+0], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
            const uint8_t  m0 = f[i+0], m1 = f[i+1], m2 = f[i+2], m3 = f[i+3];
            const uint32_t v0 = v[i+0], v1 = v[i+1], v2 = v[i+2], v3 = v[i+3];

            const uint32_t c0 = (m0 & (k0 < key_threshold) & ((k0 & 3u) == 0u)) ? v0 : 0;
            const uint32_t c1 = (m1 & (k1 < key_threshold) & ((k1 & 3u) == 0u)) ? v1 : 0;
            const uint32_t c2 = (m2 & (k2 < key_threshold) & ((k2 & 3u) == 0u)) ? v2 : 0;
            const uint32_t c3 = (m3 & (k3 < key_threshold) & ((k3 & 3u) == 0u)) ? v3 : 0;

            local += static_cast<uint64_t>(c0) + static_cast<uint64_t>(c1) +
                     static_cast<uint64_t>(c2) + static_cast<uint64_t>(c3);
        }
        for (; i < n; ++i) {
            const uint32_t ki = k[i];
            if (f[i] && ki < key_threshold && ((ki & 3u) == 0u)) {
                local += static_cast<uint64_t>(v[i]);
            }
        }
        sum = local;
    }
    return sum;
}