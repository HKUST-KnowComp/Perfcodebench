#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const size_t n = deltas.size();
    if (n == 0) return 0;

    // The decoded values are: 
    // v[0] = base + d[0]
    // v[1] = base + d[0] + d[1]
    // ...
    // v[i] = base + sum(d[0]...d[i])
    // Total sum = n * base + sum_{i=0}^{n-1} ((n - i) * d[i])

    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = static_cast<uint64_t>(n) * base;
        for (size_t i = 0; i < n; ++i) {
            current_iter_sum += static_cast<uint64_t>(deltas[i]) * (n - i);
        }
        total_sum = current_iter_sum;
    }
    return total_sum;
}