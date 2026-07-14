#include "interface.h"
#include <algorithm>
#include <immintrin.h>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const uint32_t  n = starts.size();
    const uint32_t  m = probes.size();
    if (n == 0 || m == 0 || iters <= 0) return 0;

    // build a contiguous index array so we can binary-search on starts
    // and still know the original position (needed for ends/payloads)
    std::vector<uint32_t> idx(n);
    for (uint32_t i = 0; i < n; ++i) idx[i] = i;

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;

        // unroll by 8 to expose ILP and let compiler vectorise remainder
        uint32_t i = 0;
        for (; i + 8 <= m; i += 8) {
            uint64_t psum = 0;
            #pragma unroll
            for (uint32_t k = 0; k < 8; ++k) {
                const uint32_t probe = probes[i + k];
                const auto it = std::upper_bound(starts.begin(), starts.end(), probe);
                if (it == starts.begin()) continue;                 // probe < starts[0]
                const uint32_t j = static_cast<uint32_t>(std::prev(it) - starts.begin());
                if (probe >= ends[j]) continue;                     // not inside interval
                psum += static_cast<uint64_t>(payloads[j]) * static_cast<uint64_t>(weights[i + k]);
            }
            local += psum;
        }
        // scalar tail
        for (; i < m; ++i) {
            const uint32_t probe = probes[i];
            const auto it = std::upper_bound(starts.begin(), starts.end(), probe);
            if (it == starts.begin()) continue;
            const uint32_t j = static_cast<uint32_t>(std::prev(it) - starts.begin());
            if (probe >= ends[j]) continue;
            local += static_cast<uint64_t>(payloads[j]) * static_cast<uint64_t>(weights[i]);
        }
        sum = local;
    }
    return sum;
}