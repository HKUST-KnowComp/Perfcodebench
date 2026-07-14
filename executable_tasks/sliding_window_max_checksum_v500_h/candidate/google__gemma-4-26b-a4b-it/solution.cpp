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

    const size_t n = values.size();
    const size_t w = static_cast<size_t>(window);
    const size_t num_windows = n - w + 1;
    
    // Pre-calculate the sliding window maximums to avoid redundant work across iterations
    std::vector<uint32_t> max_elements(num_windows);
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

        // If window is complete, record max
        if (i >= w - 1) {
            max_elements[i - w + 1] = values[dq.front()];
        }
    }

    uint64_t total_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_windows; ++i) {
            total_hash = mix(total_hash, max_elements[i]);
        }
    }

    return total_hash;
}