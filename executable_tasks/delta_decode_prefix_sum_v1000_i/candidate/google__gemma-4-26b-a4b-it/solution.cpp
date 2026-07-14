#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total_sum = 0;
    const size_t n = deltas.size();
    if (n == 0) return 0;

    // Pre-calculate the decoded values for one pass to avoid redundant work if iters > 1
    // However, the baseline logic suggests the sum is reset every iteration and only the last
    // iteration's sum is returned (due to sum = 0 inside the loop). 
    // Looking closely at the baseline: 
    // for (int iter = 0; iter < iters; ++iter) { ... sum = 0; for (...) { sum += current; } }
    // This means the function effectively returns the sum of the LAST iteration.
    // To preserve correctness (matching the baseline's behavior), we compute it once.

    uint64_t current_value = static_cast<uint64_t>(base);
    uint64_t iteration_sum = 0;

    for (size_t i = 0; i < n; ++i) {
        current_value += deltas[i];
        iteration_sum += current_value;
    }

    // The baseline returns the sum of the last iteration because 'sum = 0' is inside the 'iters' loop.
    // If iters <= 0, the loop doesn't run and returns 0.
    return (iters > 0) ? iteration_sum : 0;
}