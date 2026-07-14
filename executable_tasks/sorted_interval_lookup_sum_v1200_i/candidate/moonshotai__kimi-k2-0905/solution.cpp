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

    const std::size_t n_probes = probes.size();
    const std::size_t n_iv     = starts.size();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        std::size_t iv = 0;                 // current interval

        for (std::size_t pr = 0; pr < n_probes; ++pr) {
            const uint32_t probe = q[pr];
            const uint64_t weight = w[pr];

            // advance interval until it contains or overtakes probe
            while (iv < n_iv && e[iv] < probe) ++iv;

            if (iv < n_iv && s[iv] <= probe) {
                sum += static_cast<uint64_t>(p[iv]) * weight;
            }
            // else: probe < s[0] or fell off the end → contributes 0
        }
        total = sum;
    }
    return total;
}