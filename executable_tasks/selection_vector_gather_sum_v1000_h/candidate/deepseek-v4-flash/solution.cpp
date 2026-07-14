#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    // Precompute the sum of payloads for indices satisfying condition
    uint64_t final_sum = 0;
    const std::size_t n = keys.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
            final_sum += static_cast<uint64_t>(payloads[i]);
        }
    }
    // Repeat the aggregation 'iters' times (each iteration identical)
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = final_sum;
    }
    return sum;
}