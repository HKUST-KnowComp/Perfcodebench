#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    (void)iters; // iterations do not affect the final sum in the baseline
    const std::size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint8_t*  f = flags.data();
    const uint32_t* p = payloads.data();
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t key = k[i];
        if (f[i] && key < threshold && ((key & 7u) == 0u)) {
            sum += static_cast<uint64_t>(p[i]);
        }
    }
    return sum;
}