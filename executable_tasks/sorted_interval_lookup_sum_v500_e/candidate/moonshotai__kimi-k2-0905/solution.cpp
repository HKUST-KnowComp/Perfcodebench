#include "interface.h"

#include <cstdint>
#include <vector>

// Branch-free 32-bit integer upper-bound predecessor index.
// Returns the largest index such that arr[idx] <= key.
// If key < arr[0] returns 0 (same behaviour as baseline).
static inline std::size_t fast_upper_bound_pred(const uint32_t* __restrict arr,
                                                std::size_t n,
                                                uint32_t key) {
    std::size_t lo = 0, hi = n;
    while (hi - lo > 4) {
        std::size_t mid = (lo + hi) >> 1;
        if (arr[mid] <= key) lo = mid;
        else                   hi = mid;
    }
    // Linear finish to avoid mis-predicted branches
    for (; lo < hi; ++lo)
        if (arr[lo] > key) break;
    return lo - 1;
}

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    const std::size_t n = starts.size();
    const std::size_t m = probes.size();
    const uint32_t*  __restrict s = starts.data();
    const uint32_t*  __restrict p = payloads.data();
    const uint32_t*  __restrict pr = probes.data();
    const uint32_t*  __restrict w = weights.data();

    uint64_t sum = 0;
    // Peel first iteration to avoid ‘sum = 0’ inside the loop
    for (std::size_t j = 0; j < m; ++j) {
        std::size_t idx = fast_upper_bound_pred(s, n, pr[j]);
        sum += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[j]);
    }
    for (int iter = 1; iter < iters; ++iter) {
        uint64_t local = 0;
        std::size_t j = 0;
        // Unroll 4× to increase ILP
        for (; j + 3 < m; j += 4) {
            std::size_t i0 = fast_upper_bound_pred(s, n, pr[j+0]);
            std::size_t i1 = fast_upper_bound_pred(s, n, pr[j+1]);
            std::size_t i2 = fast_upper_bound_pred(s, n, pr[j+2]);
            std::size_t i3 = fast_upper_bound_pred(s, n, pr[j+3]);
            local += static_cast<uint64_t>(p[i0]) * static_cast<uint64_t>(w[j+0]);
            local += static_cast<uint64_t>(p[i1]) * static_cast<uint64_t>(w[j+1]);
            local += static_cast<uint64_t>(p[i2]) * static_cast<uint64_t>(w[j+2]);
            local += static_cast<uint64_t>(p[i3]) * static_cast<uint64_t>(w[j+3]);
        }
        for (; j < m; ++j) {
            std::size_t idx = fast_upper_bound_pred(s, n, pr[j]);
            local += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[j]);
        }
        sum = local;
    }
    return sum;
}