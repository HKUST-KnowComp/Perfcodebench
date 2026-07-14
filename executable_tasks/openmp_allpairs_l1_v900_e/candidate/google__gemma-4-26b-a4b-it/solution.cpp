#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

// Helper to compute sum of |a_i - a_j| for all i < j in O(N log N)
static int64_t compute_dim_sum(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // The term coords[i] is added i times and subtracted (n - 1 - i) times
        sum += static_cast<int64_t>(coords[i]) * (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i));
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // Precompute the sum for a single iteration
    // Since the input vectors do not change between iterations, we compute once.
    // We must copy the vectors because compute_dim_sum sorts them.
    int64_t single_iter_sum = compute_dim_sum(xs) + compute_dim_sum(ys);

    // The task specifies 'huge iters'. We can compute the total sum directly.
    // Using int128 or careful multiplication to prevent overflow if iters is massive,
    // but the return type is int64_t, implying the total fits in 64 bits.
    return static_cast<int64_t>(single_iter_sum) * iters;
}