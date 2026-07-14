#include "interface.h"

#include <cstdint>
#include <cstddef>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const uint32_t*  const __restrict ps  = probes.data();
    const uint32_t*  const __restrict ws  = weights.data();
    const uint32_t*  const __restrict ss  = starts.data();
    const uint32_t*  const __restrict es  = ends.data();
    const uint32_t*  const __restrict pls = payloads.data();
    const std::size_t n_probes = probes.size();
    const std::size_t n_iv     = starts.size();

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t j = 0;                 // current interval candidate
        for (std::size_t i = 0; i < n_probes; ++i) {
            const uint32_t q = ps[i];
            // advance while the current interval ends before the probe
            while (j < n_iv && es[j] < q) ++j;
            // if we are past the last interval or the probe is before its start, no match
            if (j < n_iv && q >= ss[j]) {
                sum += static_cast<uint64_t>(pls[j]) * static_cast<uint64_t>(ws[i]);
            }
        }
        total = sum;
    }
    return total;
}