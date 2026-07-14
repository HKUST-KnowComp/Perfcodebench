#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters)
{
    const uint32_t*  k = keys.data();
    const uint8_t*   f = flags.data();
    const uint32_t*  p = payloads.data();
    const size_t     n = keys.size();

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;

        // Manual 4-way unroll to increase ILP
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = k[i+0], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
            uint8_t  f0 = f[i+0], f1 = f[i+1], f2 = f[i+2], f3 = f[i+3];

            if (f0 && k0 < threshold && ((k0 & 7u) == 0u)) sum += p[i+0];
            if (f1 && k1 < threshold && ((k1 & 7u) == 0u)) sum += p[i+1];
            if (f2 && k2 < threshold && ((k2 & 7u) == 0u)) sum += p[i+2];
            if (f3 && k3 < threshold && ((k3 & 7u) == 0u)) sum += p[i+3];
        }
        // Tail
        for (; i < n; ++i) {
            uint32_t kv = k[i];
            if (f[i] && kv < threshold && ((kv & 7u) == 0u))
                sum += p[i];
        }
        total = sum;   // keep side-effect on iter
    }
    return total;
}