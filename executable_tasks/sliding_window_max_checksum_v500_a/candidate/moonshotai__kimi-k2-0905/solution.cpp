#include "interface.h"
#include <deque>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
    const int n = static_cast<int>(values.size());
    if (window <= 0 || window > n || iters <= 0) return 0;

    std::deque<int> dq;
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        dq.clear();
        hash = 1469598103934665603ULL;

        for (int i = 0; i < n; ++i) {
            // Pop from back while current value is greater
            while (!dq.empty() && values[i] >= values[dq.back()]) {
                dq.pop_back();
            }
            dq.push_back(i);

            // Remove indices out of the current window
            while (dq.front() <= i - window) {
                dq.pop_front();
            }

            // Once the window is fully formed, record the maximum
            if (i >= window - 1) {
                hash = mix(hash, values[dq.front()]);
            }
        }
    }
    return hash;
}