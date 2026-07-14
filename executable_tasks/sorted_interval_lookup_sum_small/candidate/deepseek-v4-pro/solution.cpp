#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    (void)iters;

    uint64_t sum = 0;
    size_t idx = 0;
    const size_t max_idx = starts.size() - 1;

    for (size_t i = 0; i < probes.size(); ++i) {
        uint32_t probe = probes[i];
        while (idx < max_idx && starts[idx + 1] <= probe) {
            ++idx;
        }
        sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }
    return sum;
}