#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    const std::size_t n = keys.size();
    if (n == 0) return 0;

    const uint32_t* __restrict k = keys.data();
    const uint8_t*  __restrict f = flags.data();
    const uint32_t* __restrict p = payloads.data();

    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // Check flag first to skip most of the work
        if (f[i]) {
            uint32_t kv = k[i];
            if (kv < threshold && ((kv & 7u) == 0u)) {
                sum += static_cast<uint64_t>(p[i]);
            }
        }
    }
    return sum;
}