#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const size_t n = starts.size();
    const size_t m = probes.size();

    // working arrays for probe order and prefix weights
    std::vector<uint32_t> idx(m);
    std::vector<uint64_t> wprefix(m + 1);
    for (size_t i = 0; i < m; ++i) idx[i] = static_cast<uint32_t>(i);

    // sort indices by probe value
    std::sort(idx.begin(), idx.end(),
              [&](uint32_t l, uint32_t r) { return probes[l] < probes[r]; });

    // build prefix sum of weights in that order
    wprefix[0] = 0;
    for (size_t i = 0; i < m; ++i)
        wprefix[i + 1] = wprefix[i] + weights[idx[i]];

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        size_t probe_idx = 0;
        for (size_t intv = 0; intv < n; ++intv) {
            const uint32_t s = starts[intv];
            const uint32_t e = ends[intv];
            const uint64_t p = payloads[intv];

            // advance to first probe >= start
            while (probe_idx < m && probes[idx[probe_idx]] < s)
                ++probe_idx;
            const size_t first = probe_idx;

            // advance to first probe > end
            while (probe_idx < m && probes[idx[probe_idx]] <= e)
                ++probe_idx;
            const size_t last = probe_idx;

            const uint64_t wsum = wprefix[last] - wprefix[first];
            total += p * wsum;
        }
    }
    return total;
}