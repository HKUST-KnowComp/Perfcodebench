#include "interface.h"
#include <deque>
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    const std::size_t n = values.size();
    if (window <= 0 || static_cast<std::size_t>(window) > n) {
        // Degenerate case: no valid windows
        uint64_t h = 1469598103934665603ULL;
        for (int iter = 0; iter < iters; ++iter) {
            h = 1469598103934665603ULL;
        }
        return h;
    }

    // Pre-compute all sliding-window maxima once
    std::vector<uint32_t> maxima;
    maxima.reserve(n - window + 1);

    std::deque<std::size_t> dq; // stores indices, values in decreasing order

    for (std::size_t i = 0; i < n; ++i) {
        // Remove indices outside the current window
        while (!dq.empty() && dq.front() + static_cast<std::size_t>(window) <= i) {
            dq.pop_front();
        }
        // Remove indices whose values are <= current value
        while (!dq.empty() && values[dq.back()] <= values[i]) {
            dq.pop_back();
        }
        dq.push_back(i);
        // The front is the maximum for the window ending at i
        if (i + 1 >= static_cast<std::size_t>(window)) {
            maxima.push_back(values[dq.front()]);
        }
    }

    // Hash the pre-computed maxima `iters` times
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t mx : maxima) {
            hash = mix(hash, mx);
        }
    }
    return hash;
}