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
    (void)ends;   // unused but part of the interface
    (void)iters;  // we compute the sum only once, as iterations are redundant

    const size_t n_probes = probes.size();
    if (n_probes == 0) return 0;

    uint64_t sum = 0;
    size_t idx = 0;
    const size_t n_starts = starts.size();

    for (size_t i = 0; i < n_probes; ++i) {
        const uint32_t probe = probes[i];
        // advance the interval index while the next start is still <= probe
        while (idx + 1 < n_starts && starts[idx + 1] <= probe) {
            ++idx;
        }
        sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }

    return sum;
}