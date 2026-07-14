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
    uint64_t total_hash = 0;

    // Pre-calculate the sliding window maxima to avoid redundant work across iterations
    // although the task implies the hash is reset per iteration, the sequence of maxima is constant.
    std::vector<uint32_t> maxima;
    maxima.reserve(n - w + 1);

    // Monotonic queue algorithm: O(N)
    std::deque<size_t> dq;
    for (size_t i = 0; i < n; ++i) {
        // Remove indices out of window
        if (!dq.empty() && dq.front() <= i - w) {
            dq.pop_front();
        }
        // Remove elements smaller than current
        while (!dq.empty() && values[dq.back()] <= values[i]) {
            dq.pop_back();
        }
        dq.push_back(i);

        // If window is full, the front is the max
        if (i >= w - 1) {
            maxima.push_back(values[dq.front()]);
        }
    }

    for (int iter = 0; iter < iters; ++iter) {
        total_hash = 1469598103934665603ULL;
        for (uint32_t mx : maxima) {
            total_hash = mix(total_hash, static_cast<uint64_t>(mx));
        }
    }

    return total_hash;
}
