#include "interface.h"

#include <cstddef>
#include <vector>

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    if (iters == 0) {
        return 0;
    }

    const std::size_t n = values.size();
    const std::size_t w = static_cast<std::size_t>(window);
    
    // If no valid windows exist, return the initial hash unchanged.
    if (w == 0 || w > n) {
        return 1469598103934665603ULL;
    }

    uint64_t hash = 1469598103934665603ULL;
    std::vector<std::size_t> q(n);
    std::size_t head = 0;
    std::size_t tail = 0;

    for (std::size_t i = 0; i < n; ++i) {
        // Maintain decreasing order in the deque (stores indices).
        while (tail > head && values[q[tail - 1]] <= values[i]) {
            --tail;
        }
        q[tail] = i;
        ++tail;

        // Remove indices that are out of the current window.
        if (q[head] + w <= i) {
            ++head;
        }

        // Once the first window is complete, the max is at the front.
        if (i + 1 >= w) {
            const uint32_t mx = values[q[head]];
            hash ^= mx;
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}
