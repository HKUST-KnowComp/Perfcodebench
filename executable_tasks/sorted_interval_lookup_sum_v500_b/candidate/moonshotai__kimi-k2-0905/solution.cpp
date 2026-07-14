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
    const std::size_t n_probes = probes.size();
    const std::size_t n_intervals = starts.size();
    std::vector<uint32_t> hit(n_probes);

    /* ---------- 1.  one-time binary search per probe ---------- */
    for (std::size_t p = 0; p < n_probes; ++p) {
        uint32_t key = probes[p];
        std::size_t lo = 0;
        std::size_t hi = n_intervals;
        while (lo < hi) {
            std::size_t mid = (lo + hi) >> 1;
            if (starts[mid] <= key) lo = mid + 1;
            else                    hi = mid;
        }
        std::size_t idx = (lo == 0 ? 0 : lo - 1);
        /* correctness: probe must lie inside the chosen interval */
        if (key <= ends[idx])
            hit[p] = payloads[idx];
        else
            hit[p] = 0;
    }

    /* ---------- 2.  repeated weighted sum ---------- */
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t* __restrict w = weights.data();
        const uint32_t* __restrict h = hit.data();
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        std::size_t i = 0;
        /* unroll 4x */
        for (; i + 4 <= n_probes; i += 4) {
            s0 += uint64_t(h[i+0]) * w[i+0];
            s1 += uint64_t(h[i+1]) * w[i+1];
            s2 += uint64_t(h[i+2]) * w[i+2];
            s3 += uint64_t(h[i+3]) * w[i+3];
        }
        /* tail */
        for (; i < n_probes; ++i)
            s0 += uint64_t(h[i]) * w[i];
        total = s0 + s1 + s2 + s3;
    }
    return total;
}