#include "interface.h"

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

        // Unroll by 4
        for (; i + 3 < n; i += 4) {
            uint32_t k0 = keys[i + 0];
            uint32_t k1 = keys[i + 1];
            uint32_t k2 = keys[i + 2];
            uint32_t k3 = keys[i + 3];

            uint8_t  f0 = flags[i + 0];
            uint8_t  f1 = flags[i + 1];
            uint8_t  f2 = flags[i + 2];
            uint8_t  f3 = flags[i + 3];

            if (f0 && (k0 < threshold) && ((k0 & 7u) == 0u)) local += payloads[i + 0];
            if (f1 && (k1 < threshold) && ((k1 & 7u) == 0u)) local += payloads[i + 1];
            if (f2 && (k2 < threshold) && ((k2 & 7u) == 0u)) local += payloads[i + 2];
            if (f3 && (k3 < threshold) && ((k3 & 7u) == 0u)) local += payloads[i + 3];
        }

        // Tail
        for (; i < n; ++i) {
            uint32_t k = keys[i];
            if (flags[i] && (k < threshold) && ((k & 7u) == 0u)) {
                local += payloads[i];
            }
        }

        sum = local;
    }
    return sum;
}