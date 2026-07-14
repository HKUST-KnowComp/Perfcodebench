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
        // 4× unroll
        for (; i + 3 < n; i += 4) {
            const uint32_t k0 = k[i+0], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
            const uint8_t  m0 = f[i+0], m1 = f[i+1], m2 = f[i+2], m3 = f[i+3];
            if (m0 && k0 < key_threshold && ((k0 & 3u) == 0u)) local += v[i+0];
            if (m1 && k1 < key_threshold && ((k1 & 3u) == 0u)) local += v[i+1];
            if (m2 && k2 < key_threshold && ((k2 & 3u) == 0u)) local += v[i+2];
            if (m3 && k3 < key_threshold && ((k3 & 3u) == 0u)) local += v[i+3];
        }
        // tail
        for (; i < n; ++i) {
            const uint32_t key = k[i];
            if (f[i] && key < key_threshold && ((key & 3u) == 0u))
                local += v[i];
        }
        sum = local;
    }
    return sum;
}