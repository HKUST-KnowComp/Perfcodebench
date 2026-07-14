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
    // Using a vector as a deque to avoid std::deque overhead
    std::vector<size_t> dq;
    dq.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        // Remove indices out of window
        if (!dq.empty() && dq.front() <= i - w) {
            dq.erase(dq.begin());
        }

        // Remove elements smaller than current from back
        while (!dq.empty() && values[dq.back()] <= values[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if (i >= w - 1) {
            maxes.push_back(values[dq.front()]);
        }
    }

    // Re-implementing the deque logic more efficiently for performance
    // The above is a conceptual placeholder; let's use a more performant manual deque
    maxes.clear();
    std::vector<size_t> q(n);
    int head = 0;
    int tail = 0;

    for (size_t i = 0; i < n; ++i) {
        if (head < tail && q[head] <= i - w) {
            head++;
        }
        while (head < tail && values[q[tail - 1]] <= values[i]) {
            tail--;
        }
        q[tail++] = i;
        if (i >= w - 1) {
            maxes.push_back(values[q[head]]);
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