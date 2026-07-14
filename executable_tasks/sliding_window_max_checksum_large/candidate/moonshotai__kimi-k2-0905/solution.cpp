#include "interface.h"
#include <vector>
#include <cstdint>
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// pre-compute all sliding-window maxima in O(n) time
static std::vector<uint32_t> build_window_maxima(
    const std::vector<uint32_t>& a,
    int W)
{
    const int n = static_cast<int>(a.size());
    if (n < W || W <= 0) return {};
    std::vector<uint32_t> out;
    out.reserve(n - W + 1);

    // monotonic deque (circular buffer, never reallocates)
    std::vector<int> q(n);
    int qh = 0, qt = 0;

    for (int i = 0; i < n; ++i) {
        // pop from back while smaller than current
        while (qt > qh && a[q[qt - 1]] <= a[i]) --qt;
        q[qt++] = i;

        // pop from front if outside window
        while (q[qh] <= i - W) ++qh;

        // first valid window completed
        if (i >= W - 1) {
            out.push_back(a[q[qh]]);
        }
    }
    return out;
}

} // anonymous namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values,
                                int window,
                                int iters)
{
    if (window <= 0 || window > static_cast<int>(values.size()))
        return 0;

    const auto maxima = build_window_maxima(values, window);
    if (maxima.empty()) return 0;

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        for (uint32_t v : maxima)
            hash = mix(hash, v);
    }
    return hash;
}