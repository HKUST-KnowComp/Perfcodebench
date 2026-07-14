#include "interface.h"
#include <cstdint>
#include <vector>
#include <deque>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    const int n = static_cast<int>(values.size());
    if (window <= 0 || window > n || iters <= 0) return 0;

    std::vector<uint32_t> maxima;
    maxima.reserve(n - window + 1);

    std::deque<int> dq;

    for (int i = 0; i < n; ++i) {
        while (!dq.empty() && dq.front() <= i - window)
            dq.pop_front();
        while (!dq.empty() && values[dq.back()] <= values[i])
            dq.pop_back();
        dq.push_back(i);
        if (i >= window - 1)
            maxima.push_back(values[dq.front()]);
    }

    const uint64_t seed = 1469598103934665603ULL;
    const int m = static_cast<int>(maxima.size());

    uint64_t hash = seed;
    for (int iter = 0; iter < iters; ++iter) {
        hash = seed;
        const uint32_t* __restrict p = maxima.data();
        int k = 0;
        for (; k + 7 < m; k += 8) {
            hash = mix(hash, p[k]);
            hash = mix(hash, p[k + 1]);
            hash = mix(hash, p[k + 2]);
            hash = mix(hash, p[k + 3]);
            hash = mix(hash, p[k + 4]);
            hash = mix(hash, p[k + 5]);
            hash = mix(hash, p[k + 6]);
            hash = mix(hash, p[k + 7]);
        }
        for (; k < m; ++k)
            hash = mix(hash, p[k]);
    }
    return hash;
}