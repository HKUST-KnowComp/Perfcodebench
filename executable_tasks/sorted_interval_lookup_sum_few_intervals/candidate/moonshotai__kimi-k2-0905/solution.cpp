#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const std::size_t n = probes.size();
    const std::size_t m = starts.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t j = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t p = probes[i];
            while (j < m && ends[j] < p) ++j;
            sum += static_cast<uint64_t>(payloads[j]) * static_cast<uint64_t>(weights[i]);
        }
    }
    return sum;
}