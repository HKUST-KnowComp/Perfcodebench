#include "interface.h"
#include <deque>
#include <vector>

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    size_t n = values.size();
    size_t w = static_cast<size_t>(window);
    std::vector<uint32_t> maxs;
    if (w > 0 && w <= n) {
        maxs.reserve(n - w + 1);
        std::deque<size_t> dq;
        for (size_t i = 0; i < n; ++i) {
            while (!dq.empty() && dq.front() + w <= i)
                dq.pop_front();
            while (!dq.empty() && values[dq.back()] <= values[i])
                dq.pop_back();
            dq.push_back(i);
            if (i >= w - 1) {
                maxs.push_back(values[dq.front()]);
            }
        }
    }
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t mx : maxs) {
            hash ^= static_cast<uint64_t>(mx);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}