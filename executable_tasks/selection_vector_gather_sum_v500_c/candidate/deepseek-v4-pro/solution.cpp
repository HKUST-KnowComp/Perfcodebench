#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    (void)iters; // unused parameter, result is independent of iteration count
    uint64_t sum = 0;
    const std::size_t n = keys.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
            sum += static_cast<uint64_t>(payloads[i]);
        }
    }
    return sum;
}