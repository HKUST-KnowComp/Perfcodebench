#include "interface.h"
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    const std::size_t n = keys.size();
    const uint32_t* keyData = keys.data();
    const uint8_t* flagData = flags.data();
    const uint32_t* payloadData = payloads.data();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t localSum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (flagData[i] && (keyData[i] < threshold) && ((keyData[i] & 7u) == 0u)) {
                localSum += payloadData[i];
            }
        }
        sum = localSum;
    }
    return sum;
}
