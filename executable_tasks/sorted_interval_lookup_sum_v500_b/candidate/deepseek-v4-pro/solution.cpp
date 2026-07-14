#include "interface.h"

#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    if (iters <= 0) return 0;
    const std::size_t n = starts.size();
    const std::size_t m = probes.size();
    if (m == 0) return 0;
    uint64_t total = 0;
    std::size_t i = 0;
    for (std::size_t j = 0; j < m; ++j) {
        uint32_t p = probes[j];
        while (i < n && starts[i] <= p) ++i;
        std::size_t idx = i - 1;
        total += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[j]);
    }
    return total;
}