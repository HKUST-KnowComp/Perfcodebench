#include "interface.h"
#include <deque>

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    const std::size_t n = values.size();
    const std::size_t w = static_cast<std::size_t>(window);

    // Edge case: window = 0 (empty window), maximum is 0 for each start index
    if (w == 0) {
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
            for (std::size_t i = 0; i < n; ++i) {
                hash ^= 0;
                hash *= 1099511628211ULL;
            }
        }
        return hash;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        std::deque<std::size_t> q;
        for (std::size_t i = 0; i < n; ++i) {
            // Remove index if it falls out of the current window
            if (!q.empty() && q.front() + w <= i) q.pop_front();
            // Maintain decreasing order: remove elements smaller or equal
            while (!q.empty() && values[q.back()] <= values[i]) q.pop_back();
            q.push_back(i);
            // Once the first full window is formed, emit the maximum
            if (i + 1 >= w) {
                std::uint32_t mx = values[q.front()];
                hash ^= mx;
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}