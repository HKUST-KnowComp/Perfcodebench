#include "interface.h"
#include <cstddef>
#include <vector>

namespace {
    inline uint64_t mix(uint64_t hash, uint64_t value) {
        hash ^= value;
        hash *= 1099511628211ULL;
        return hash;
    }
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    const std::size_t n = values.size();
    const int w = window;

    // Preallocate index array for the monotonic queue
    std::vector<std::size_t> dq(n);
    uint64_t hash = 0;

    // Handle edge cases with window <= 0
    if (w <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
            for (std::size_t i = 0; i < n; ++i) {
                hash = mix(hash, 0);
            }
        }
        return hash;
    }

    // No windows when window > n
    if (static_cast<std::size_t>(w) > n) {
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
        }
        return hash;
    }

    // Fast path: 0 < w <= n
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        std::size_t head = 0, tail = 0;

        for (std::size_t i = 0; i < n; ++i) {
            // Remove indices that are out of the current window
            while (head < tail && dq[head] + w <= i) {
                ++head;
            }
            // Remove indices whose values are not greater than the current value
            while (head < tail && values[dq[tail - 1]] <= values[i]) {
                --tail;
            }
            dq[tail++] = i;

            // We have a full window from i-w+1 to i
            if (i + 1 >= static_cast<std::size_t>(w)) {
                uint64_t mx = values[dq[head]];
                hash ^= mx;
                hash *= 1099511628211ULL;
            }
        }
    }

    return hash;
}