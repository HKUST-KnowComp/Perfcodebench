#include "interface.h"
#include <deque>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // anonymous namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values,
                                 int window,
                                 int iters) {
    const std::size_t n = values.size();
    const std::size_t w = static_cast<std::size_t>(window);
    if (w == 0 || w > n) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;

        std::deque<std::size_t> dq;

        for (std::size_t i = 0; i < n; ++i) {
            // Remove indices outside the current window
            while (!dq.empty() && dq.front() <= i - w) {
                dq.pop_front();
            }

            // Remove indices whose values are <= current value
            while (!dq.empty() && values[dq.back()] <= values[i]) {
                dq.pop_back();
            }

            dq.push_back(i);

            // Once the window is fully formed, the front is the maximum
            if (i >= w - 1) {
                hash = mix(hash, static_cast<uint64_t>(values[dq.front()]));
            }
        }
    }
    return hash;
}