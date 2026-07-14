#include "interface.h"
#include <vector>
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    if (values.empty() || window <= 0 || static_cast<size_t>(window) > values.size()) {
        return 0;
    }

    size_t n = values.size();
    size_t w = static_cast<size_t>(window);
    std::vector<uint32_t> maxes;
    maxes.reserve(n - w + 1);

    // Monotonic queue to find sliding window maximums in O(N)
    // Stores indices of elements in 'values'
    std::deque<size_t> dq;

    for (size_t i = 0; i < n; ++i) {
        // Remove indices that are out of the current window
        if (!dq.empty() && dq.front() <= i - w) {
            dq.pop_front();
        }

        // Remove elements smaller than the current element from the back
        while (!dq.empty() && values[dq.back()] <= values[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        // If we have reached the first full window, record the max
        if (i >= w - 1) {
            maxes.push_back(values[dq.front()]);
        }
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t mx : maxes) {
            hash = mix(hash, mx);
        }
    }

    return hash;
}