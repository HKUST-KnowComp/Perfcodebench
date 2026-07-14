#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t sum = 0;
    const std::size_t n = keys.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
            sum += payloads[i];
        }
    }
    return sum;
}
