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
    const uint32_t*  s = starts.data();
    const uint32_t*  e = ends.data();
    const uint32_t*  p = payloads.data();
    const uint32_t*  q = probes.data();
    const uint32_t*  w = weights.data();

    const std::size_t n_intervals = starts.size();
    const std::size_t n_probes    = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t j = 0;                 // interval cursor
        for (std::size_t i = 0; i < n_probes; ++i) {
            const uint32_t probe = q[i];
            const uint64_t weight = w[i];

            // advance interval until probe <= ends[j]
            while (j < n_intervals && probe > e[j]) ++j;

            // if probe is inside the current interval, accumulate
            const bool inside = (j < n_intervals && probe >= s[j]);
            sum += inside ? (static_cast<uint64_t>(p[j]) * weight) : 0ULL;
        }
    }
    return sum;
}