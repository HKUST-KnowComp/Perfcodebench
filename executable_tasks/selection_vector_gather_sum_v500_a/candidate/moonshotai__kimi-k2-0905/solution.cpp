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
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        std::size_t i = 0;

        // unroll 4x
        for (; i + 3 < n; i += 4) {
            const uint32_t k0 = keys[i + 0];
            const uint32_t k1 = keys[i + 1];
            const uint32_t k2 = keys[i + 2];
            const uint32_t k3 = keys[i + 3];

            const bool ok0 = flags[i + 0] && (k0 < threshold) && ((k0 & 7u) == 0u);
            const bool ok1 = flags[i + 1] && (k1 < threshold) && ((k1 & 7u) == 0u);
            const bool ok2 = flags[i + 2] && (k2 < threshold) && ((k2 & 7u) == 0u);
            const bool ok3 = flags[i + 3] && (k3 < threshold) && ((k3 & 7u) == 0u);

            if (ok0) local += payloads[i + 0];
            if (ok1) local += payloads[i + 1];
            if (ok2) local += payloads[i + 2];
            if (ok3) local += payloads[i + 3];
        }
        // tail
        for (; i < n; ++i) {
            const uint32_t k = keys[i];
            if (flags[i] && (k < threshold) && ((k & 7u) == 0u))
                local += payloads[i];
        }
        sum = local;
    }
    return sum;
}