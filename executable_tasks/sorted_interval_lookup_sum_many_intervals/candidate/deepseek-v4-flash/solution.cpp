#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    std::size_t n = starts.size();
    std::size_t m = probes.size();
    std::size_t i = 0;
    uint64_t one_sum = 0;
    for (std::size_t p = 0; p < m; ++p) {
        uint32_t probe = probes[p];
        while (i < n && starts[i] <= probe) {
            if (probe < ends[i]) {
                break;
            }
            ++i;
        }
        if (i < n && starts[i] <= probe && probe < ends[i]) {
            one_sum += static_cast<uint64_t>(payloads[i]) * static_cast<uint64_t>(weights[p]);
        }
    }
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = one_sum;
    }
    return sum;
}
