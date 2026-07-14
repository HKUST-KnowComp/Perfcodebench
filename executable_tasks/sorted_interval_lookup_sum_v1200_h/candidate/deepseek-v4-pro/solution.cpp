#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    if (iters <= 0) return 0;

    const uint32_t* p_starts = starts.data();
    const uint32_t* p_payloads = payloads.data();
    const uint32_t* p_probes = probes.data();
    const uint32_t* p_weights = weights.data();
    const std::size_t n_starts = starts.size();
    const std::size_t n_probes = probes.size();

    std::size_t j = 0;
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n_probes; ++i) {
        uint32_t probe = p_probes[i];
        while (j + 1 < n_starts && p_starts[j + 1] <= probe) {
            ++j;
        }
        sum += static_cast<uint64_t>(p_payloads[j]) * static_cast<uint64_t>(p_weights[i]);
    }
    return sum;
}
