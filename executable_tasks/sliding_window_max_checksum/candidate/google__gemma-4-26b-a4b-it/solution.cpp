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

    // Monotonic deque stores indices
    // Using a vector as a manual deque for performance
    std::vector<size_t> dq;
    dq.reserve(n);
    size_t head = 0;
    size_t tail = 0;

    for (size_t i = 0; i < n; ++i) {
        // Remove indices out of window
        if (tail > head && dq[head] <= i - w) {
            head++;
        }

        // Remove elements smaller than current from back
        while (tail > head && values[dq[tail - 1]] <= values[i]) {
            tail--;
        }

        dq[tail++] = i;

        if (i >= w - 1) {
            maxes.push_back(values[dq[head]]);
        }
    }

    uint64_t total_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = 1469598103934665603ULL;
        for (uint32_t mx : maxes) {
            current_hash = mix(current_hash, mx);
        }
        total_hash = current_hash;
    }

    return total_hash;
}