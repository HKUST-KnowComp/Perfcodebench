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
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t kv = k[i];
            if (f[i] && kv < threshold && ((kv & 7u) == 0u)) {
                local += static_cast<uint64_t>(p[i]);
            }
        }
        sum = local;
    }
    return sum;
}