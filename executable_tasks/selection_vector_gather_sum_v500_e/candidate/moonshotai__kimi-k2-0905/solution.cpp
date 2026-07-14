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
    const uint32_t*  k  = keys.data();
    const uint8_t*   f  = flags.data();
    const uint32_t*  p  = payloads.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;

        std::size_t i = 0;
        // 4-way unrolled main loop
        for (; i + 3 < n; i += 4) {
            const uint32_t k0 = k[i+0];
            const uint32_t k1 = k[i+1];
            const uint32_t k2 = k[i+2];
            const uint32_t k3 = k[i+3];

            const bool m0 = f[i+0] && (k0 < threshold) && ((k0 & 7u) == 0u);
            const bool m1 = f[i+1] && (k1 < threshold) && ((k1 & 7u) == 0u);
            const bool m2 = f[i+2] && (k2 < threshold) && ((k2 & 7u) == 0u);
            const bool m3 = f[i+3] && (k3 < threshold) && ((k3 & 7u) == 0u);

            local += m0 ? static_cast<uint64_t>(p[i+0]) : 0;
            local += m1 ? static_cast<uint64_t>(p[i+1]) : 0;
            local += m2 ? static_cast<uint64_t>(p[i+2]) : 0;
            local += m3 ? static_cast<uint64_t>(p[i+3]) : 0;
        }
        // tail
        for (; i < n; ++i) {
            const uint32_t kv = k[i];
            if (f[i] && (kv < threshold) && ((kv & 7u) == 0u)) {
                local += static_cast<uint64_t>(p[i]);
            }
        }
        sum = local;   // final sum from last iteration
    }
    return sum;
}